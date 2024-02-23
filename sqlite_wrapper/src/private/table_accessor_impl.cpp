/*
 * table_accessor_impl.cpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#include "private/table_accessor_impl.hpp"
#include "private/sqlite_call.hpp"
#include "private/bind_fields_as_parameters_visitor.hpp"
#include "private/bind_parameter.hpp"
#include "private/record_iterator_impl.hpp"

namespace sqlw
{

//---------------------------------------------------------------------------------------------
TableAccessorImpl::TableAccessorImpl(
		SqliteHandle< sqlite3 > const & _db
	, 	std::string const & _tableName
	, 	bool _assumeView
)
	:	m_db( _db )
	,	m_tableName( _tableName )
	,	m_view( _assumeView )
{
}

//---------------------------------------------------------------------------------------------
TableAccessorImpl::~TableAccessorImpl()
{
}

//---------------------------------------------------------------------------------------------
bool
TableAccessorImpl::reset( sqlite3_stmt *const a_stmt ) const
{
	if( a_stmt == nullptr )
		return false;

	sqliteCall< sqlite3_reset >( a_stmt );
	sqliteCall< sqlite3_clear_bindings >( a_stmt );
	return true;
}

//*****************************************************************
int
TableAccessorImpl::size()
{
	auto stmt = prepare( "SELECT COUNT( id ) FROM $TABLE;" );

	sqliteCall< sqlite3_step >( stmt );

	return sqlite3_column_int( stmt, 0 );
}

//---------------------------------------------------------------------------------------------
std::int64_t
TableAccessorImpl::getLastInsertId()
{
	auto stmt = prepare( "SELECT ID FROM $TABLE ORDER BY ID DESC LIMIT 1;" );

	sqliteCall< sqlite3_step >( stmt );

	return sqlite3_column_int64( stmt, 0 );
}

//---------------------------------------------------------------------------------------------
void
TableAccessorImpl::prepareUpdate( IRecordWrapper const & _record )
{
	bool first = true;

	auto columnNames = _record.getColumnNames( true );

	std::string sql = "UPDATE \"" + m_tableName + "\" SET ";

	for( std::string const & column : columnNames )
	{
		if( !first )
			sql += ",";

		first = false;

		sql += column + " = ?";
	}

	sql += " WHERE ID = ?;";
	sqliteCall< sqlite3_prepare >( m_db, sql.c_str(), -1, &m_stmtUpdate, nullptr );
}

//---------------------------------------------------------------------------------------------
void
TableAccessorImpl::prepareInsert( IRecordWrapper const & _record )
{
	bool first = true;

	auto columnNames = _record.getColumnNames( true );

	std::string sql = "INSERT INTO " + m_tableName + "(";

	std::string sqlTail = "VALUES(";

	for( std::string const & column : columnNames )
	{
		if( !first )
		{
			sql += ",";
			sqlTail += ",";
		}

		first = false;
		sql += column;
		sqlTail += "?";
	}

	sql += ") ";
	sqlTail += ")";

	sql += sqlTail + ";";

	sqliteCall< sqlite3_prepare >( m_db, sql.c_str(), -1, &m_stmtInsert, nullptr );
}

//---------------------------------------------------------------------------------------------
void
TableAccessorImpl::insert( IRecordWrapper& _record, bool _reload )
{
	if( !reset( m_stmtInsert ) )
		prepareInsert( _record );

	BindFieldsAsParametersVisitor bindVisitor( m_stmtInsert, false );

	_record.accept( bindVisitor );

    // DBTransaction trans(m_pdb);
	// INSERT
	sqliteCall< sqlite3_step >( m_stmtInsert );
	// trans.commit();
	if( _reload )
		_record.setId( getLastInsertId() );
}

//---------------------------------------------------------------------------------------------
void
TableAccessorImpl::update( IRecordWrapper& _record )
{
	if( !reset( m_stmtUpdate ) )
		prepareUpdate( _record );


	BindFieldsAsParametersVisitor bindVisitor( m_stmtUpdate, true );

	_record.accept( bindVisitor );

	// UPDATE

	int indexOfId = bindVisitor.getNextBindIndex();

	if( indexOfId > 1 )
	{
		sqliteCall< sqlite3_bind_int >( m_stmtUpdate, indexOfId, _record.getId() );
		sqliteCall< sqlite3_step >( m_stmtUpdate );
	}
}

//---------------------------------------------------------------------------------------------
void
TableAccessorImpl::save( IRecordWrapper& _record, bool _reloadAfterInsert )
{
	if( _record.getId() == -1 )
	{
		insert( _record, _reloadAfterInsert );
	}
	else
	{
		update(_record );
	}
}

//---------------------------------------------------------------------------------------------
void
TableAccessorImpl::remove( std::int64_t _nId )
{
	auto stmt = prepare( "DELETE FROM $TABLE WHERE ID = ?;" );

	bindParameter( stmt, 1, _nId );

	sqliteCall< sqlite3_step >( stmt );
}

//---------------------------------------------------------------------------------------------
bool
TableAccessorImpl::find( std::int64_t _nId, IRecordWrapper& _record )
{
	auto stmt = prepare( "SELECT * FROM $TABLE WHERE ID = ?;" );

	bindParameter( stmt, 1, _nId );

	if( sqlite3_step( stmt ) == SQLITE_ROW )
	{
		ColumnDataProvider columnDataProvider;

		columnDataProvider.load( stmt, _record );
		_record.setId( _nId );

		return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------------
IRecordIteratorHandle
TableAccessorImpl::getAll()
{
	auto stmt = prepare( "SELECT * FROM $TABLE;" );

	return RecordIteratorImpl::create( stmt );
}

//---------------------------------------------------------------------------------------------
IRecordIteratorHandle
TableAccessorImpl::findBy( std::string const & _columnName, std::type_info const * const _type, ... )
{
	std::string sql = "SELECT * FROM " + m_tableName + " WHERE " + _columnName + " = ?;";

	auto stmt = prepare( sql );

	va_list args;
	va_start( args, _type );

	if( _type == &typeid( std::int64_t ) )
		bindParameter( stmt, 1, va_arg( args, int ) );
	else if( _type == &typeid( double ) )
		bindParameter( stmt, 1, va_arg( args, double ) );
	else if( _type == &typeid( std::string ) )
		bindParameter( stmt, 1, *va_arg( args, std::string* ) );

	return RecordIteratorImpl::create( stmt );
}

//---------------------------------------------------------------------------------------------
sqlite3_stmt * const
TableAccessorImpl::prepare( std::string const & _sql )
{
	auto it = m_preparedStmtMap.find( _sql );
	if( it != m_preparedStmtMap.end() )
	{
		sqliteCall< sqlite3_reset >( it->second );
		sqliteCall< sqlite3_clear_bindings >( it->second );
		return it->second;
	}

	std::string sqlParsed = _sql;

	static std::string const Pattern = "$TABLE";
	auto index = sqlParsed.find( Pattern );

	if( index != std::string::npos )
		sqlParsed.replace( index, Pattern.size(), m_tableName );

	SqliteHandle< sqlite3_stmt > stmt;

	sqliteCall< sqlite3_prepare >( m_db, sqlParsed.c_str(), -1, &stmt, nullptr );

	m_preparedStmtMap[ _sql ] = stmt;

	return stmt;
}


} /* namespace sqlw */

