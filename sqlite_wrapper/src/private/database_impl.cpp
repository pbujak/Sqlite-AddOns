/*
 * database_impl.cpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#include "private/database_impl.hpp"
#include "private/table_accessor_impl.hpp"
#include "private/query_impl.hpp"

namespace sqlw
{

//-------------------------------------------------------------------------
DatabaseImpl::DatabaseImpl( SqliteHandle< sqlite3 > && _db )
	:	m_db( std::move( _db ) )
{
}

//-------------------------------------------------------------------------
DatabaseImpl::~DatabaseImpl()
{
}

//-------------------------------------------------------------------------
ITableAccessorHandle
DatabaseImpl::openTable( std::string const & _tableName, bool _assumeView )
{
	return std::make_shared< TableAccessorImpl >( m_db, _tableName, _assumeView );
}

//-------------------------------------------------------------------------
IQueryHandle
DatabaseImpl::openQuery( std::string const & _tableName, std::string const & _whereClause )
{
	return QueryImpl::create( m_db, _tableName, _whereClause );
}

} /* namespace sqlw */
