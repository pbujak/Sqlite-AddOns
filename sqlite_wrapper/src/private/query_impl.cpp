/*
 * query_impl.cpp
 *
 *  Created on: 14 lut 2024
 *      Author: supcio
 */

#include "private/query_impl.hpp"
#include "private/sqlite_call.hpp"
#include "private/bind_parameter.hpp"
#include "private/record_iterator_impl.hpp"

namespace sqlw
{

//----------------------------------------------------------------------------------------
QueryImpl::QueryImpl( SqliteHandle< sqlite3 > const & _db, std::string const & _table, std::string const & _sql )
	:	m_db( _db )
	,	m_tableName( _table )
	,	m_sql( _sql )
{
	// TODO Auto-generated constructor stub

}

//----------------------------------------------------------------------------------------
QueryImpl::~QueryImpl()
{
	// TODO Auto-generated destructor stub
}

//----------------------------------------------------------------------------------------
std::shared_ptr< QueryImpl >
QueryImpl::create(
		SqliteHandle< sqlite3 > const & _db
	, 	std::string const & _table
	,	std::string const & _whereClause
	)
{
	std::string sql = "SELECT * FROM " + _table + " WHERE " + _whereClause;

	auto result = std::make_shared< QueryImpl >( _db, _table, sql );

	result->prepare();

	return result;
}

//----------------------------------------------------------------------------------------
void
QueryImpl::reset()
{
	m_nextIndex = 1;

	if( m_stmt.getUseCount() > 1 )
	{
		m_stmt.reset();
		prepare();
	}
	else
	{
		sqliteCall< sqlite3_reset >( m_stmt );
		sqliteCall< sqlite3_clear_bindings >( m_stmt );
	}
}

//----------------------------------------------------------------------------------------
void
QueryImpl::prepare()
{
	if( !m_stmt )
		sqliteCall< sqlite3_prepare >( m_db, m_sql.c_str(), -1, &m_stmt, nullptr );
}

//----------------------------------------------------------------------------------------
template< typename _Parameter >
void
QueryImpl::bindParameterImpl( _Parameter const & _parameter, std::string const & _name )
{
	int index;

	if( !_name.empty() )
		index = sqlite3_bind_parameter_index( m_stmt, _name.c_str() );
	else
		index = m_nextIndex++;

	sqlw::bindParameter( m_stmt, index, _parameter );
}

//----------------------------------------------------------------------------------------
void
QueryImpl::bindParameter( std::int64_t _int, std::string const & _name )
{
	bindParameterImpl( _int, _name );
}

//----------------------------------------------------------------------------------------
void
QueryImpl::bindParameter( double _double, std::string const & _name )
{
	bindParameterImpl( _double, _name );
}

//----------------------------------------------------------------------------------------
void
QueryImpl::bindParameter( std::string const & _string, std::string const & _name )
{
	bindParameterImpl( _string, _name );
}

//----------------------------------------------------------------------------------------
IRecordIteratorHandle
QueryImpl::getAll()
{
	return RecordIteratorImpl::create( m_stmt );
}

} /* namespace sqlw */

