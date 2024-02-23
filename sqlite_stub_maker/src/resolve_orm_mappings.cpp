/*
 * resolve_orm_mappings.cpp
 *
 *  Created on: 23 sty 2024
 *      Author: supcio
 */
#include "resolve_orm_mappings.hpp"
#include "error_handling.hpp"
#include "db_schema_object_cache.hpp"
#include <algorithm>

namespace
{

using CheckFirstColumnFunction = void ( * )( int _line, DbTableColumn const & _column );

//==============================================================================
struct SourceColumnInfo
{
	DbTableColumn m_dbColumn;
	bool          m_modifiable;
};

//==============================================================================
struct SourceTableInfo
{
	std::string m_name;

	DbSchemaObject::Kind m_kind;

	bool m_modifiable;

	CheckFirstColumnFunction m_checkFirstColumn;

	std::vector< SourceColumnInfo > m_columns;
};


//------------------------------------------------------------------------------
template< typename ... _MessageFragments >
void
throwError( int _line, _MessageFragments const & ... _fragments )
{
	ErrorHandling::throwError(
			ErrorInfo( "ORM", Util::formatMessage( _fragments ... ) )
				.setLocation( _line )
		);
}

//------------------------------------------------------------------------------
void
updateViewModifiable( SourceTableInfo & _sourceTable, DbSchemaObjectCache & _cache )
{
	_sourceTable.m_modifiable = false;

	for( auto & column : _sourceTable.m_columns )
		column.m_modifiable = false;

	bool modifiableAllColumns = false;

	std::set< std::string, CompareNoCase > modifiableColumns;

	auto triggers = _cache.getTriggers( _sourceTable.m_name );

	for( DbParsedTrigger const & trigger : triggers )
	{
		if( trigger.m_moment != DbParsedTrigger::Moment::InsteadOf )
			continue;

		if(
				trigger.m_kind == DbParsedTrigger::Kind::Insert
			||  trigger.m_kind == DbParsedTrigger::Kind::Delete
			)
		{
			_sourceTable.m_modifiable = true;
		}
		else if( trigger.m_kind == DbParsedTrigger::Kind::Update )
		{
			if( trigger.m_updateColumns.empty() )
				modifiableAllColumns = true;
			else
			{
				for( auto const & column : trigger.m_updateColumns )
					modifiableColumns.insert( column );
			}
		}
	}

	if( !_sourceTable.m_modifiable )
		return;

	for( auto & column : _sourceTable.m_columns )
	{
		if( modifiableAllColumns )
		{
			column.m_modifiable = true;
			continue;
		}

		if( modifiableColumns.find( column.m_dbColumn.m_name ) != modifiableColumns.end() )
			column.m_modifiable = true;
	}
}

//------------------------------------------------------------------------------
void
checkFirstTableOrViewColumn( int _line, DbTableColumn const & _column )
{
	if(
			!Util::equalsNoCase( _column.m_name, "ID" )
		||	!Util::equalsNoCase( _column.m_type, "INTEGER" )
		)
	{
		throwError( _line, "First column is expected to be named ID with type INTEGER." );
	}
}

//------------------------------------------------------------------------------
void
checkFirstTableColumn( int _line, DbTableColumn const & _column )
{
	checkFirstTableOrViewColumn( _line, _column );

	if( !_column.m_primaryKey )
		throwError( _line, "First column is expected to be a primary key." );
}

//------------------------------------------------------------------------------
SourceTableInfo
getSourceTableInfo(
		int _line
	,	std::string const & _table
	,	InMemorySqliteDatabase const & _database
	,	DbSchemaObjectCache & _cache
	)
{
	auto table = _cache.getTable( _table );
	if( !table )
		throwError( _line, "Unresolved table/view ", Util::quote( _table ), "." );

	SourceTableInfo sourceTableInfo;
	sourceTableInfo.m_name = table->m_name;
	sourceTableInfo.m_kind = table->m_kind;
	sourceTableInfo.m_modifiable = true;

	auto tableInfo = _database.getTableInfo( _table );

	for( DbTableColumn & dbcolumn : tableInfo )
	{
		SourceColumnInfo column;
		column.m_dbColumn = std::move( dbcolumn );
		column.m_modifiable = true;
		sourceTableInfo.m_columns.push_back( std::move( column ) );
	}

	if( table->m_kind == DbSchemaObject::Kind::View )
	{
		updateViewModifiable( sourceTableInfo, _cache );
		sourceTableInfo.m_checkFirstColumn = &checkFirstTableOrViewColumn;
	}
	else
		sourceTableInfo.m_checkFirstColumn = &checkFirstTableColumn;

	return sourceTableInfo;
}


//------------------------------------------------------------------------------
std::type_info const *
getCppType( std::string const & _dbType )
{
	static std::map< std::string, std::type_info const *, CompareNoCase > mapDbType
		=	{
					{ "INTEGER", &typeid( std::int64_t ) }
				,	{ "REAL", &typeid( double ) }
				,	{ "TEXT", &typeid( std::string ) }
			};

	auto it = mapDbType.find( _dbType );
	if( it == mapDbType.end() )
		return nullptr;

	return it->second;
}

//--------------------------------------------------------------------------------------------
void
fillFieldData( bool _isview, FieldData & _target, SourceColumnInfo const & _column )
{
	_target.m_name = _column.m_dbColumn.m_name;
	_target.m_cppType = getCppType( _column.m_dbColumn.m_type );

	if( !_target.m_cppType )
	{
		throwError(
				_target.m_xmlOrmLine
			, 	"Database type "
			, 	Util::quote( _column.m_dbColumn.m_type )
			, 	"is not supported."
		);

	}

	_target.m_modifiable = _column.m_modifiable;

	if( boost::indeterminate( _target.m_notNull ) )
		_target.m_notNull = _column.m_dbColumn.m_notNull;
	else if( !_isview )
		throwError( _target.m_xmlOrmLine, "Attribute \"notnull\" is supported only for SQL views." );
}

//--------------------------------------------------------------------------------------------
void
resolveMappingsForTable(
		TableData & _ormTable
	,	InMemorySqliteDatabase const & _database
	,	DbSchemaObjectCache & _cache
	)
{
	SourceTableInfo dbtable
		= getSourceTableInfo( _ormTable.m_xmlOrmLine, _ormTable.m_name, _database, _cache );

	_ormTable.m_name = dbtable.m_name;
	_ormTable.m_isView = dbtable.m_kind == DbSchemaObject::Kind::View;
	_ormTable.m_modifiable = dbtable.m_modifiable;

	dbtable.m_checkFirstColumn( _ormTable.m_xmlOrmLine, dbtable.m_columns[ 0 ].m_dbColumn );

	for( auto itColumn = dbtable.m_columns.begin() + 1; itColumn != dbtable.m_columns.end(); ++itColumn )
	{
		SourceColumnInfo const & column = *itColumn;

		auto it = _ormTable.m_xmlRawFields.find( column.m_dbColumn.m_name );

		if( it == _ormTable.m_xmlRawFields.end() )
		{
			std::string fullColumnName = dbtable.m_name + "." + column.m_dbColumn.m_name;

			throwError(
					_ormTable.m_xmlOrmLine
				, 	"Missing mapping for column "
				,	Util::quote( fullColumnName )
				,	'.'
				);
		}

		FieldData fieldData = std::move( *it );
		_ormTable.m_xmlRawFields.erase( it );
		fillFieldData( _ormTable.m_isView, fieldData, column );
		_ormTable.m_resolvedFields.push_back( std::move( fieldData ) );
	}

	if( !_ormTable.m_xmlRawFields.empty() )
	{
		FieldData const & fieldData = *_ormTable.m_xmlRawFields.begin();

		std::string fullColumnName = dbtable.m_name + "." + fieldData.m_name;

		throwError(
				fieldData.m_xmlOrmLine
			, 	"Unresolved column "
			,	Util::quote( fullColumnName )
			,	'.'
			);
	}
}

}

//--------------------------------------------------------------------------------------------
void
resolveOrmMappings( OrmMappingsData & _ormMappings, InMemorySqliteDatabase const & _database )
{
	ScopedContextFile contextFile( _ormMappings.m_sourceXmlFile );

	DbSchemaObjectCache cache( &_database );

	for( TableData & tableData : _ormMappings.m_tables )
	{
		resolveMappingsForTable( tableData, _database, cache );
	}
}


