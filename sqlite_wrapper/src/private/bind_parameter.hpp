/*
 * bind_parameter.hpp
 *
 *  Created on: 12 lut 2024
 *      Author: supcio
 */

#pragma once
#include "private/sqlite_call.hpp"
#include <sqlite3.h>
#include <type_traits>
#include <string>
#include <optional>

namespace sqlw
{

//-----------------------------------------------------------------------
template< typename _Parameter >
inline void
bindParameter(
		sqlite3_stmt * const _stmt
	,	int _index
	,	_Parameter const & _parameter
	)
{
	if constexpr( std::is_convertible_v< _Parameter, std::string > )
	{
		sqliteCall< sqlite3_bind_text >(
				_stmt
			, 	_index
			, 	_parameter.c_str()
			, 	-1
			,	SQLITE_TRANSIENT
			);
	}
	else if constexpr ( std::is_convertible_v< _Parameter, std::int64_t > )
		sqliteCall< sqlite3_bind_int64 >( _stmt, _index, _parameter );

	else if constexpr ( std::is_convertible_v< _Parameter, double > )
		sqliteCall< sqlite3_bind_double >( _stmt, _index, _parameter );
}

//-----------------------------------------------------------------------
template< typename _Parameter >
inline void
bindNullableParameter(
		sqlite3_stmt * const _stmt
	,	int _index
	,	std::optional< _Parameter > const & _parameter
	)
{
	if( _parameter.has_value() )
		bindParameter( _stmt, _index, *_parameter );
	else
		sqliteCall< sqlite3_bind_null >( _stmt, _index );
}


}
