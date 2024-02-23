/*
 * query_impl.hpp
 *
 *  Created on: 14 lut 2024
 *      Author: supcio
 */

#pragma once
#include "private/sqlite_handle.hpp"
#include "i_query.hpp"
#include <sqlite3.h>

namespace sqlw
{

//=======================================================================
class QueryImpl : public IQuery
{
public:
	static std::shared_ptr< QueryImpl > create( SqliteHandle< sqlite3 > const & _db, std::string const & _table, std::string const & _whereClause );

	QueryImpl( SqliteHandle< sqlite3 > const & _db, std::string const & _table, std::string const & _sql );

	virtual ~QueryImpl();

	void reset() override;

	void bindParameter( std::int64_t _int, std::string const & _name ) override;

	void bindParameter( double _double, std::string const & _name ) override;

	void bindParameter( std::string const & _string, std::string const & _name ) override;

	IRecordIteratorHandle getAll() override;

private:
	void prepare();

	template< typename _Parameter >
	void bindParameterImpl( _Parameter const & _parameter, std::string const & _name );

private:
	SqliteHandle< sqlite3 > m_db;

	SqliteHandle< sqlite3_stmt > m_stmt;

	std::string m_tableName;
	std::string m_sql;

	int m_nextIndex = 1;
};

} /* namespace sqlw */
