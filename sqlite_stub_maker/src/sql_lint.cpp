/*
 * sql_lint.cpp
 *
 *  Created on: 16 sty 2024
 *      Author: supcio
 */
#include "sql_lint.hpp"
#include "sql_analyzer.hpp"
#include "error_handling.hpp"
#include "db_object_extract.hpp"
#include "db_schema_object_cache.hpp"
#include "util.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>

//--------------------------------------------------------------------------------
SqlLint::SqlLint( InMemorySqliteDatabase * _database, std::function< int( int ) > const & _getLineFromOffset )
	:	m_database( _database )
	,	m_getLineFromOffset( _getLineFromOffset )
{
}

//--------------------------------------------------------------------------------
void
SqlLint::analyze( int _offset, std::vector< SqlToken > const & _sqlCommand )
{
	SqlAnalyzer analyzer( _sqlCommand );

	if( analyzeInsert( _offset, analyzer ) )
		return;

	if( analyzeDelete( _offset, analyzer ) )
		return;

	if( analyzeUpdate( _offset, analyzer ) )
		return;

	if( analyzer.nextTokensLike( { "ALTER" } ) )
		warning( _offset, "There is no reason to use ALTER in database template definition." );

	else if( analyzer.nextTokensLike( { "DROP" } ) )
	{
		warning( _offset, "There is no reason to use DROP in database template definition." );

		analyzeDropTrigger( _offset, analyzer );
	}
	else if( analyzer.nextTokensLike( { "CREATE" } ) )
		analyzeCreate( _offset, analyzer );
}

//--------------------------------------------------------------------------------
bool
SqlLint::analyzeInsert( int _offset, SqlAnalyzer & _analyzer )
{
	if( !isCommandRegardingWith( _analyzer, "INSERT" ) )
		return false;

	if( m_mapTableToUntestedTrigger.empty() )
		return true;

	if( _analyzer.nextTokensLike( { "OR" } ) )
		_analyzer.nextIdentifier();

	if( !_analyzer.nextTokensLike( { "INTO" } ) )
		return true;

	std::string table = *_analyzer.nextIdentifier();

	auto eqr = m_mapTableToUntestedTrigger.equal_range( table );

	for( auto it = eqr.first; it != eqr.second; ++it )
	{
		if( it->m_trigger.m_kind == DbParsedTrigger::Kind::Insert )
		{
			m_mapTableToUntestedTrigger.erase( it );
			break;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
bool
SqlLint::analyzeDelete( int _offset, SqlAnalyzer & _analyzer )
{
	if( !isCommandRegardingWith( _analyzer, "DELETE" ) )
		return false;

	if( m_mapTableToUntestedTrigger.empty() )
		return true;

	if( !_analyzer.nextTokensLike( { "FROM" } ) )
		return true;

	std::string table = *_analyzer.nextIdentifier();

	auto eqr = m_mapTableToUntestedTrigger.equal_range( table );

	for( auto it = eqr.first; it != eqr.second; ++it )
	{
		if( it->m_trigger.m_kind == DbParsedTrigger::Kind::Delete )
		{
			m_mapTableToUntestedTrigger.erase( it );
			break;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
bool
SqlLint::analyzeUpdate( int _offset, SqlAnalyzer & _analyzer )
{
	if( !isCommandRegardingWith( _analyzer, "UPDATE" ) )
		return false;

	if( m_mapTableToUntestedTrigger.empty() )
		return true;

	if( _analyzer.nextTokensLike( { "OR" } ) )
		_analyzer.nextIdentifier();

	std::string table = *_analyzer.nextIdentifier();

	if( !_analyzer.nextTokensLike( { "SET" } ) )
		return true;

	std::set< std::string, CompareNoCase > touchedTriggers;

	auto eqr = m_mapTableToUntestedTrigger.equal_range( table );

	for( auto it = eqr.first; it != eqr.second; ++it )
	{
		if( it->m_trigger.m_kind != DbParsedTrigger::Kind::Update )
			continue;

		if( it->m_trigger.m_updateColumns.empty() )
		{
			touchedTriggers.insert( it->m_trigger.m_name );
			continue;
		}

		for( std::string const & column : it->m_trigger.m_updateColumns )
		{
			SqlAnalyzer localAnalyzer( _analyzer );

			if( localAnalyzer.findAndJumpAfter( { column, "=" } ) )
				touchedTriggers.insert( it->m_trigger.m_name );
		}
	}

	while( true )
	{
		auto it = std::find_if(
					m_mapTableToUntestedTrigger.begin()
				,	m_mapTableToUntestedTrigger.end()
				,	[ &touchedTriggers ]( auto const & _triggerInfo )
					{
						return touchedTriggers.find( _triggerInfo.m_trigger.m_name ) != touchedTriggers.end();
					}
				);

		if( it == m_mapTableToUntestedTrigger.end() )
			break;

		m_mapTableToUntestedTrigger.erase( it );
	}

	return true;
}

//--------------------------------------------------------------------------------
void
SqlLint::analyzeCreate( int _offset, SqlAnalyzer & _analyzer )
{
	if(
			_analyzer.nextTokensLike( { "TEMP" } )
		||	_analyzer.nextTokensLike( { "TEMPORARY" } )
		)
	{
		warning( _offset, "Temporary object looks like unintended in in database template definition." );
	}

	if( _analyzer.nextTokensLike( { "TABLE" } ) )
	{
		_analyzer.nextTokensLike( { "IF", "NOT", "EXISTS" } );

		auto name = _analyzer.nextIdentifier();
		if( !name )
			return;

		if( !_analyzer.findAndJumpAfter( { "PRIMARY", "KEY" } ) )
		{
			warning(
					_offset
				, 	std::string( "Table " ) + Util::quote( *name ) + " has no primary key."
				);
		}
		return;
	}

	if( _analyzer.nextTokensLike( { "VIEW" } ) )
	{
		analyzeView( _offset, *_analyzer.getUnderlyingTokens() );
		return;
	}

	if( analyzeTrigger( _offset, _analyzer ) )
		return;
}

//--------------------------------------------------------------------------------
void
SqlLint::analyzeView( int _offset, std::vector< SqlToken > const & _sqlCommand )
{
	DbSchemaObjectCache cache( m_database );

	auto isTable = [ &cache ]( std::string const & _table ) -> bool
	{
		return cache.getTable( _table ).has_value();
	};

	auto parsedView = dbExtractView( _sqlCommand, isTable );
	if( !parsedView )
		return;

	for( auto [ tableName, column ] : parsedView->m_joinColumns )
	{
		auto table = cache.getTable( tableName );
		if( !table || table->m_kind != DbSchemaObject::Kind::Table )
			continue;

		if( !cache.isColumnIndexed( tableName, column ) )
		{
			std::stringstream ss;

			ss << "No index on column \"" << tableName << '.' << column << "\".";
			warning( _offset, ss.str() );
		}
	}
}

//--------------------------------------------------------------------------------
bool
SqlLint::analyzeTrigger( int _offset, SqlAnalyzer & _analyzer )
{
	if( ! _analyzer.nextTokensLike( { "TRIGGER" } ) )
		return false;

	auto trigger = dbExtractTrigger( *_analyzer.getUnderlyingTokens() );

	if( !trigger )
		return true;

	UntouchedTrigger untouchedTrigger;

	untouchedTrigger.m_offset = _offset;
	untouchedTrigger.m_trigger = *trigger;

	m_mapTableToUntestedTrigger.insert( std::move( untouchedTrigger ) );

	return true;
}

//--------------------------------------------------------------------------------
bool
SqlLint::analyzeDropTrigger( int _offset, SqlAnalyzer & _analyzer )
{
	if( !_analyzer.nextTokensLike( { "TRIGGER" } ) )
		return false;

	_analyzer.nextTokensLike( { "IF", "EXISTS" } );

	std::string triggerName = *_analyzer.nextIdentifier();

	auto it = std::find_if(
			m_mapTableToUntestedTrigger.begin()
		,	m_mapTableToUntestedTrigger.end()
		,	[ triggerName ]( auto const & _tableAndTrigger )
			{
				return Util::equalsNoCase( triggerName, _tableAndTrigger.m_trigger.m_name );
			}
		);

	if( it != m_mapTableToUntestedTrigger.end() )
		m_mapTableToUntestedTrigger.erase( it );

	return true;
}

//--------------------------------------------------------------------------------
bool
SqlLint::isCommandRegardingWith( SqlAnalyzer & _analyzer, std::string const & _command )
{
	if( _analyzer.nextTokensLike( { _command } ) )
		return true;

	if( !_analyzer.nextTokensLike( { "WITH" }, false ) )
		return false;

	return _analyzer.findAndJumpAfter( { _command } );
}

//--------------------------------------------------------------------------------
void
SqlLint::checkForeignKeyIndexes() const
{
	auto tables = m_database->getTablesOrViews();

	DbSchemaObjectCache cache( m_database );

	for( auto const & table : tables )
	{
		if( table.m_kind != DbSchemaObject::Kind::Table )
			continue;

		auto foreignKeys = m_database->getForeignKeyList( table.m_name );

		for( auto const & foreignKey : foreignKeys )
		{
			if( !cache.isColumnIndexed( table.m_name, foreignKey.m_columnLocal ) )
			{
				std::stringstream ss;

				ss << "No index on column \"" << table.m_name << '.' << foreignKey.m_columnLocal << "\" with foreign key.";
				warning( -1, ss.str() );

			}
		}
	}
}

//--------------------------------------------------------------------------------
void
SqlLint::checkUntouchedTriggers() const
{
	for( auto const & tableAndTrigger : m_mapTableToUntestedTrigger )
	{
		std::stringstream ss;

		ss << "Trigger " << Util::quote( tableAndTrigger.m_trigger.m_name ) << " is completely untouched, so its behaviour is not tested.";

		warning( tableAndTrigger.m_offset, ss.str() );
	}
}

//--------------------------------------------------------------------------------
void
SqlLint::checkIncompleteTriggers() const
{
	auto found = []( auto const & _container, auto const & _condition )
		{
			auto end = _container.end();

			return std::find_if( _container.begin(), end, _condition ) != end;
		};

	auto isInsteadOf = []( DbParsedTrigger const & _trigger ) -> bool
		{
			return _trigger.m_moment == DbParsedTrigger::Moment::InsteadOf;
		};

	auto isInsert = [ isInsteadOf ]( DbParsedTrigger const & _trigger )
		{
			return
					_trigger.m_kind == DbParsedTrigger::Kind::Insert
				&& 	isInsteadOf( _trigger );
		};

	auto isDelete = [ isInsteadOf ]( DbParsedTrigger const & _trigger )
		{
			return
					_trigger.m_kind == DbParsedTrigger::Kind::Delete
				&& 	isInsteadOf( _trigger );
		};

	auto showWarning = [ this ]( std::string const & _view, std::string const & _missing )
		{
			std::stringstream ss;
			ss
				<< "Incomplete triggers on view "
				<< Util::quote( _view )
				<< ". Missing " << _missing << ".";

			warning( -1, ss.str() );
		};

	auto tables = m_database->getTablesOrViews();

	DbSchemaObjectCache cache( m_database );

	for( auto const & table : tables )
	{
		if( table.m_kind != DbSchemaObject::Kind::View )
			continue;

		auto triggers = cache.getTriggers( table.m_name );

		if( !found( triggers, isInsteadOf ) )
			continue;

		if( !found( triggers, isInsert ) )
			showWarning( table.m_name, "INSTEAD OF INSERT" );

		if( !found( triggers, isDelete ) )
			showWarning( table.m_name, "INSTEAD OF DELETE" );
	}
}

//--------------------------------------------------------------------------------
void
SqlLint::checkNotEmptyTables() const
{
	auto tables = m_database->getTablesOrViews();

	for( auto const & table : tables )
	{
		if( table.m_kind != DbSchemaObject::Kind::Table )
			continue;

		if( m_database->getTableRowCount( table.m_name ) > 0 )
		{
			warning( -1, "Table ", Util::quote( table.m_name )," is not empty." );
		}

	}
}

//--------------------------------------------------------------------------------
void
SqlLint::checkFinal() const
{
	checkForeignKeyIndexes();
	checkUntouchedTriggers();
	checkIncompleteTriggers();
	checkNotEmptyTables();
}

//--------------------------------------------------------------------------------
template< typename ... _MessageFragments >
void
SqlLint::warning( int _offset,  _MessageFragments const & ... _messageFragments ) const
{
	ErrorHandling::reportWarning(
			ErrorInfo( "SQL", Util::formatMessage( _messageFragments ... ) )
				.setLocation( _offset == -1 ? -1 : m_getLineFromOffset( _offset ) )
		);
}

