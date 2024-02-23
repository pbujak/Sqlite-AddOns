/*
 * sql_engine.hpp
 *
 *  Created on: 26 sty 2024
 *      Author: supcio
 */

#pragma once
#include "db_auxiliary.hpp"
#include "in_memory_sqlite_database.hpp"
#include <unordered_map>

//==============================================================
class SqlEngine
{
public:
	SqlEngine( InMemorySqliteDatabase * _database );

	virtual ~SqlEngine();

	void execute( SqlCommand const & _sqlCommand );

private:
	std::unordered_map< std::string, SqlValue > extractParameters( SqlCommand const & _sqlCommand );

	bool executeAssignment(
			SqlCommand const & _sqlCommand
		,   std::unordered_map< std::string, SqlValue > const & _parameters
		);

private:
	InMemorySqliteDatabase * m_database;

	std::unordered_map< std::string, SqlValue > m_variableDictionary;
};

