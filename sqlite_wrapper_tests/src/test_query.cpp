/*
 * test_query.cpp
 *
 *  Created on: 16 lut 2024
 *      Author: supcio
 */

#include "test_base.hpp"
#include "test_base.hpp"
#include "db_unit_record.hpp"
#include "binary_database_template.hpp"
#include "resources.hpp"
#include "equals_iterator_or_table.hpp"

#include "../../sqlite_wrapper/src/open_database.hpp"
#include "../../sqlite_wrapper/src/db_exception.hpp"
#include "../../sqlite_wrapper/src/db_query.hpp"


namespace
{

//----------------------------------------------------------------
sqlw::IDatabaseHandle initialize()
{
	std::remove( "/tmp/database.db3" );

	BinaryDatabaseTemplate bdt( Resources::Db3Template );

	auto database = sqlw::openDatabase( "/tmp/database.db3", sqlw::FlagsReadWrite, &bdt );

	sqlw::DbTable< dbSymbols::DbUnitRecord > dbUnit( database );

	auto inserter = [ &dbUnit ]( std::string const & _name )
		{
			dbSymbols::DbUnitRecord record;
			record.m_name = _name;
			dbUnit.save( std::move( record ) );
		};


	inserter( "Name1" );
	inserter( "Name2" );
	inserter( "Name3" );
	inserter( "Name4" );

	inserter( "UName1" );
	inserter( "UName2" );
	inserter( "UName3" );
	inserter( "UName4" );

	return database;
}

}

//----------------------------------------------------------------
UNIT_TEST( TestQuery, NamePrefix )
{
	auto database = initialize();

	sqlw::DbQuery< dbSymbols::DbUnitRecord > dbQuery( database, "name like ? || '%'" );

	auto it = dbQuery.bindParameter( "Name" ).getAll();

	ASSERT_TRUE(
			equalsIterator(
					std::move( it )
				, 	{ { 1, "Name1" }, { 2, "Name2" }, { 3, "Name3" }, { 4, "Name4" } }
				)
			)

	dbQuery.reset().bindParameter( "UName" );
	it = dbQuery.getAll();

	ASSERT_TRUE(
			equalsIterator(
					std::move( it )
				, 	{ { 5, "UName1" }, { 6, "UName2" }, { 7, "UName3" }, { 8, "UName4" } }
				)
			)}
