/*
 * test_base.cpp
 *
 *  Created on: 6 lut 2024
 *      Author: supcio
 */
#include "test_base.hpp"
#include <iostream>
#include <map>

UnitTest const * UnitTest::ms_first = nullptr;

//--------------------------------------------------------------------------------------------
UnitTest::UnitTest( std::string const & _group, std::string const & _test, TestFunction _testFunction )
	:	m_next( ms_first )
	,	m_group( _group )
	,	m_test( _test )
	,	m_testFunction( _testFunction )
{
	ms_first = this;
}

//--------------------------------------------------------------------------------------------
void
UnitTest::runAll()
{
	std::map< std::string, UnitTest const* > testList;

	for( auto next = ms_first; next; next = next->m_next )
	{
		testList[ next->m_group + ":" + next->m_test ] = next;
	}

	std::cout << std::endl;

	for( auto const & [ key, test ] : testList )
	{
		bool verdict = true;

		try
		{
			( test->m_testFunction )();
		}
		catch( std::exception & e )
		{
			std::cout << e.what() << std::endl;
			verdict = false;
		}

		std::cout
			<< test->m_group << ':' << test->m_test << "... "
			<< ( verdict ? "PASSED" : "FAILED" )
			<< std::endl;
	}
	std::cout << std::endl;
}
