/*
 * sqlite_call.hpp
 *
 *  Created on: 4 lut 2024
 *      Author: supcio
 */

#pragma once
#include "db_exception.hpp"
#include <utility>
#include <sqlite3.h>


namespace sqlw
{

//----------------------------------------------------------------------------
template< auto _Function, typename _RawSqliteHandle, typename ... _More >
int
sqliteCall( _RawSqliteHandle _sqliteHandle, _More && ... _more )
{
	int result = _Function( _sqliteHandle, std::forward< _More >( _more ) ... );

	if( result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE )
		return result;

	if constexpr( std::is_convertible_v< _RawSqliteHandle, sqlite3* > )
	{
		throw DbException(
				sqlite3_errmsg( _sqliteHandle )
			,	DbException::Category::Sqlite
			,	result
			,	sqlite3_extended_errcode( _sqliteHandle )
			);
	}
	else if constexpr( std::is_convertible_v< _RawSqliteHandle, sqlite3_stmt* > )
	{
		sqlite3* db = sqlite3_db_handle( _sqliteHandle );

		throw DbException(
				sqlite3_errmsg( db )
			,	DbException::Category::Sqlite
			,	result
			,	sqlite3_extended_errcode( db )
			);
	}
	else
	{
		throw DbException(
				sqlite3_errstr( result )
			,	DbException::Category::Sqlite
			,	result
			,	result
			);
	}
}


}
