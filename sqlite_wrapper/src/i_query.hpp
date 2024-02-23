/*
 * i_query.hpp
 *
 *  Created on: 14 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_record_iterator.hpp"
#include <string>
#include <memory>

namespace sqlw
{

//========================================================================================
struct IQuery
{
	virtual ~IQuery() = default;

	virtual void reset() = 0;

	virtual void bindParameter( std::int64_t _int, std::string const & _name ) = 0;

	virtual void bindParameter( double _double, std::string const & _name ) = 0;

	virtual void bindParameter( std::string const & _string, std::string const & _name ) = 0;

	virtual IRecordIteratorHandle getAll() = 0;
};

using IQueryHandle = std::shared_ptr< IQuery >;

}

