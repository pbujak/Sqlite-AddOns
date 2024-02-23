/*
 * database_impl.h
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_database.hpp"
#include "private/sqlite_handle.hpp"
#include <sqlite3.h>


namespace sqlw
{

//============================================================
class DatabaseImpl : public IDatabase
{
public:
	DatabaseImpl( SqliteHandle< sqlite3 > && _db );

	virtual ~DatabaseImpl();

	ITableAccessorHandle openTable( std::string const & _tableName, bool _assumeView ) override;

	IQueryHandle openQuery( std::string const & _tableName, std::string const & _whereClause ) override;

private:
	SqliteHandle< sqlite3 > m_db;
};

} /* namespace sqlw */
