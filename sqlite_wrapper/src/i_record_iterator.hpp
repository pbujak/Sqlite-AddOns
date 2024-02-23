/*
 * i_record_iterator.hpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_record_wrapper.hpp"
#include <memory>

namespace sqlw
{

//====================================================================================
struct IRecordIterator
{
	virtual ~IRecordIterator() = default;

	virtual bool hasNext() const = 0;

	virtual void next( IRecordWrapper & _record ) = 0;

};

using IRecordIteratorHandle = std::shared_ptr< IRecordIterator >;

}
