/*
 * split_sql_into_commands.hpp
 *
 *  Created on: 15 sty 2024
 *      Author: supcio
 */

#pragma once
#include "sql_token.hpp"
#include "db_auxiliary.hpp"
#include <vector>
#include <string>
#include <set>


SqlCommandOrderedSet
splitSqlIntoCommands( std::string const & _sql );

