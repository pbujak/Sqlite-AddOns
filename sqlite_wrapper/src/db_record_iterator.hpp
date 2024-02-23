/*
 * db_record_iterator.hpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_record_iterator.hpp"
#include "record_wrapper_impl.hpp"

namespace sqlw
{

//========================================================================================
template< class _Record >
class DbRecordIterator
{
public:
	DbRecordIterator( IRecordIteratorHandle const & _handle )
		:	m_handle( _handle )
	{
	};

	bool hasNext() const
	{
		return m_handle->hasNext();
	}

	_Record next()
	{
		_Record result;
		m_handle->next( *getRecordWraper( &result ) );
		return result;
	}

private:
	IRecordIteratorHandle m_handle;
};

}
