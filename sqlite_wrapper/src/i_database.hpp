/*
 * database.hpp
 *
 *  Created on: 1 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_table_accessor.hpp"
#include "i_query.hpp"
#include <memory>

namespace sqlw
{

//===========================================================================
struct IDatabase
{
	virtual ~IDatabase() = default;

	virtual ITableAccessorHandle openTable( std::string const & _tableName, bool _assumeView ) = 0;

	virtual IQueryHandle openQuery( std::string const & _tableName, std::string const & _whereClause ) = 0;
};

using IDatabaseHandle = std::shared_ptr< IDatabase >;

}



