/*
 * i_table_accessor.hpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_record_wrapper.hpp"
#include "i_record_iterator.hpp"
#include <memory>

namespace sqlw
{

//================================================================
struct ITableAccessor
{
	virtual ~ITableAccessor() = default;

	virtual int size() = 0;

	virtual void save( IRecordWrapper& _record, bool _reloadAfterInsert ) = 0;

	virtual void remove( std::int64_t _nId ) = 0;

	virtual bool find( std::int64_t _nId, IRecordWrapper& _record ) = 0;

	virtual IRecordIteratorHandle getAll() = 0;

	virtual IRecordIteratorHandle findBy( std::string const & _columnName, std::type_info const * const _type, ... ) = 0;
};

using ITableAccessorHandle = std::shared_ptr< ITableAccessor >;

}
