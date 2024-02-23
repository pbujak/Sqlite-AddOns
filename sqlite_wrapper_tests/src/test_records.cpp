/*
 * test_records.cpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */


#include "test_base.hpp"
#include "db_unit_record.hpp"
#include "binary_database_template.hpp"
#include "resources.hpp"
#include "equals_iterator_or_table.hpp"

#include "../../sqlite_wrapper/src/open_database.hpp"
#include "../../sqlite_wrapper/src/db_table.hpp"

#include <algorithm>

//----------------------------------------------------------------
UNIT_TEST( TestRecords, InsertUpdate )
{
	std::remove( "/tmp/database.db3" );

	BinaryDatabaseTemplate bdt( Resources::Db3Template );

	auto database = sqlw::openDatabase( "/tmp/database.db3", sqlw::FlagsReadWrite, &bdt );

	sqlw::DbTable< dbSymbols::DbUnitRecord > dbUnit( database );

	dbSymbols::DbUnitRecord record1;
	record1.m_name = "Name1";
	dbUnit.save( std::move( record1 ) );

	record1.m_name = "Name2";
	dbUnit.save( record1 );

	ASSERT_TRUE( dbUnit.size() == 2 )

	ASSERT_TRUE( equalsTable( dbUnit, { { 1, "Name1" }, { 2, "Name2" } } ) )

	record1.m_name = "Name22";
	dbUnit.save( record1 );
	ASSERT_TRUE( equalsTable( dbUnit, { { 1, "Name1" }, { 2, "Name22" } } ) )
}


//----------------------------------------------------------------
UNIT_TEST( TestRecords, Find )
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

	ASSERT_TRUE( dbUnit.size() == 4 )

	ASSERT_TRUE(
			equalsTable(
					dbUnit
				, 	{ { 1, "Name1" }, { 2, "Name2" }, { 3, "Name3" }, { 4, "Name4" }   }
			)
		)

	std::optional< dbSymbols::DbUnitRecord > found = dbUnit.find( 3 );

	ASSERT_TRUE( found )
	ASSERT_TRUE( found->m_name == "Name3" )
}

//----------------------------------------------------------------
UNIT_TEST( TestRecords, FindByName )
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

	ASSERT_TRUE( dbUnit.size() == 4 )

	ASSERT_TRUE(
			equalsTable(
					dbUnit
				, 	{ { 1, "Name1" }, { 2, "Name2" }, { 3, "Name3" }, { 4, "Name4" }   }
			)
		)

	auto found = dbUnit.findBy< &dbSymbols::DbUnitRecord::m_name >( "Name3" );

	ASSERT_TRUE(
			equalsIterator(
					std::move( found )
				, 	{ { 3, "Name3" } }
			)
		)
}

//----------------------------------------------------------------
UNIT_TEST( TestRecords, Delete )
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

	ASSERT_TRUE( dbUnit.size() == 4 )

	ASSERT_TRUE(
			equalsTable(
					dbUnit
				, 	{ { 1, "Name1" }, { 2, "Name2" }, { 3, "Name3" }, { 4, "Name4" }   }
			)
		)

	dbUnit.remove( 2 );

	ASSERT_TRUE(
			equalsTable(
					dbUnit
				, 	{ { 1, "Name1" }, { 3, "Name3" }, { 4, "Name4" }   }
			)
		)
}
