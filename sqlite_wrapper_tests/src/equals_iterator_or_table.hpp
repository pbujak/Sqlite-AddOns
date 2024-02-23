/*
 * equals_iterator_or_table.hpp
 *
 *  Created on: 16 lut 2024
 *      Author: supcio
 */

#pragma once
#include "../../sqlite_wrapper/src/db_table.hpp"
#include <vector>
#include <algorithm>

//----------------------------------------------------------------
template< typename _Record >
bool
equalsIterator( sqlw::DbRecordIterator< _Record > && _itdb, std::vector< _Record > _expected )
{
	while( _itdb.hasNext() )
	{
		_Record record = _itdb.next();

		auto it = std::find_if(
				_expected.begin()
			, 	_expected.end()
			,	[ &record ]( _Record &_r2 )
				{
					return record.m_id == _r2.m_id;
				}
			);

		if( it == _expected.end() )
			return false;
		if( !( *it == record ) )
			return false;

		_expected.erase( it );
	}

	return _expected.empty();
}

//----------------------------------------------------------------
template< typename _Record >
bool
equalsTable( sqlw::DbTable< _Record > const & _dbtable, std::vector< _Record > const & _expected )
{
	return equalsIterator( _dbtable.getAll(), _expected );
}



