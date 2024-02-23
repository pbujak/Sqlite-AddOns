/*
 * db_result.hpp
 *
 *  Created on: 4 lut 2024
 *      Author: supcio
 */

#pragma once
#include <sqlite3.h>

namespace sqlw
{

void dbResult( int res );

void dbResult( sqlite3 *const pdb, int res );

}
