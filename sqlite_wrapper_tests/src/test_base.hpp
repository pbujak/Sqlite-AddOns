/*
 * tests.hpp
 *
 *  Created on: 6 lut 2024
 *      Author: supcio
 */

#pragma once
#include <string>
#include <sstream>
#include <exception>

using TestFunction = void ( * )();

//===============================================
class UnitTest
{
public:
	UnitTest( std::string const & _group, std::string const & _test, TestFunction _testFunction );

	static void runAll();

private:
	static UnitTest const * ms_first;

	UnitTest const * const m_next = nullptr;

	std::string m_group;
	std::string m_test;

	TestFunction m_testFunction;
};

#define UNIT_TEST( group, name ) \
	static void UnitTestFunction##group##name(); \
	static UnitTest s_unitTest##group##name( #group, #name, &UnitTestFunction##group##name ); \
	static void UnitTestFunction##group##name()

#define ASSERT_TRUE( x ) \
	if( !(x) ) \
	{ \
		std::stringstream ss; \
		ss <<__FILE__ << ':' << __LINE__ << ": assertion \"" << #x << "\" failed.";\
		throw std::logic_error( ss.str() );\
	}

#define ASSERT_FALSE( x ) \
	if( (x) ) \
	{ \
		std::stringstream ss; \
		ss <<__FILE__ << ':' << __LINE__ << ": assertion \"" << #x << "\" failed.";\
		throw std::logic_error( ss.str() );\
	}
