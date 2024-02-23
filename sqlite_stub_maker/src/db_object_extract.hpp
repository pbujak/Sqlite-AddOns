/*
 * db_object_extract.hpp
 *
 *  Created on: 17 sty 2024
 *      Author: supcio
 */

#pragma once
#include "db_objects.hpp"
#include "sql_token.hpp"
#include <functional>
#include <optional>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
std::optional< DbParsedView >
dbExtractView(
		std::vector< SqlToken > const & _tokens
	, 	std::function< bool( std::string const & ) > const & _isKnownTable
	);

std::optional< DbParsedTrigger > dbExtractTrigger( std::vector< SqlToken > const & _sqlCommand );

std::optional< DbParsedTrigger > dbExtractTrigger( std::string const & _sqlCommand );
