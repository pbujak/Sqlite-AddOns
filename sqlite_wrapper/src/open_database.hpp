/*
 * open_database.hpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_database.hpp"
#include "i_database_template.hpp"

namespace sqlw
{

int constexpr FlagsReadOnly = 0x1;
int constexpr FlagsReadWrite = 0x2;
int constexpr FlagsCreate = 0x4;


IDatabaseHandle openDatabase(
		std::string const & _file
	,	int _flags = FlagsReadWrite | FlagsCreate
	,	IDatabaseTemplate const * const _template = nullptr
	);

}



