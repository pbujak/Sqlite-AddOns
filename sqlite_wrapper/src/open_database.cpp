/*
 * open_database.cpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */
#include "private/database_impl.hpp"
#include "private/sqlite_handle.hpp"
#include "private/sqlite_call.hpp"
#include "open_database.hpp"
#include "db_exception.hpp"
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sqlite3.h>


namespace sqlw
{

namespace
{

//--------------------------------------------------------------------------
void
createFileFromTemplate(
		std::string const & _file
	,	IDatabaseTemplate const * const _template
	)
{
	std::ofstream file( _file.c_str(), std::ios::out | std::ios::binary );

	file.write(
			reinterpret_cast< char const* >( _template->getBinaryData() )
		, 	_template->getBinaryDataSize()
		);

	if( !file.good() )
		throw DbException( std::strerror( errno ), DbException::Category::Template, errno, -1 );
}

}

//--------------------------------------------------------------------------
IDatabaseHandle
openDatabase(
		std::string const & _file
	,	int _flags
	,	IDatabaseTemplate const * const _template
	)
{
	SqliteHandle< sqlite3 > db;

	int access = 0;

	if( _flags & FlagsReadOnly )
		access |= SQLITE_OPEN_READONLY;

	if( _flags & FlagsReadWrite )
		access |= SQLITE_OPEN_READWRITE;

	if( sqlite3_open_v2( _file.c_str(), &db, access, nullptr ) == SQLITE_OK )
		return std::make_shared< DatabaseImpl >( std::move( db ) );

	if( _template && _template->getTemplateKind() == IDatabaseTemplate::Kind::Binary )
		createFileFromTemplate( _file, _template );

	sqliteCall< sqlite3_open_v2 >(_file.c_str(), &db, access | SQLITE_OPEN_CREATE, nullptr );

	if( _template && _template->getTemplateKind() == IDatabaseTemplate::Kind::SqlSchema )
	{
		sqliteCall< sqlite3_exec >(
				db
			,	_template->getSqlSchema().c_str()
			,	nullptr
			,	nullptr
			,	nullptr
			);

	}

	return std::make_shared< DatabaseImpl >( std::move( db ) );
}

}

