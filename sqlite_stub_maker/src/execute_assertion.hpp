/*
 * execute_assertion.hpp
 *
 *  Created on: 25 sty 2024
 *      Author: supcio
 */

#pragma once

#include "in_memory_sqlite_database.hpp"
#include "split_sql_into_commands.hpp"
#include <unordered_map>

//------------------------------------------------------------------------------
bool
executeAssertion(
		InMemorySqliteDatabase & _inMemoryDatabase
	, 	SqlCommand const & _sqlCommand
	, 	std::unordered_map< std::string, SqlValue > const & parameters
	);


