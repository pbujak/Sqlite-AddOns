/*
 * test_open.cpp
 *
 *  Created on: 6 lut 2024
 *      Author: supcio
 */
#include "test_base.hpp"
#include "binary_database_template.hpp"
#include "resources.hpp"

#include "../../sqlite_wrapper/src/open_database.hpp"
#include "../../sqlite_wrapper/src/db_exception.hpp"

//=====================================================================
UNIT_TEST( TestOpen, DirectoryInsteadOfFile )
{
	try
	{
		auto database = sqlw::openDatabase( "~/sql-source", sqlw::FlagsReadWrite, nullptr );
	}
	catch( sqlw::DbException & e )
	{
		return;
	}
	ASSERT_TRUE( true )
}

//=====================================================================
UNIT_TEST( TestOpen, OpenWithTemplate )
{
	BinaryDatabaseTemplate bdt( Resources::Db3Template );

	auto database = sqlw::openDatabase( "/tmp/database.db3", sqlw::FlagsReadWrite, &bdt );
}
