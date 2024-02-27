/*
 * arguments.cpp
 *
 *  Created on: 7 sty 2024
 *      Author: supcio
 */
#include "arguments.hpp"
#include "util.hpp"
#include <string>

namespace
{

//================================================================
class ArgumentIterator
{
public:
	ArgumentIterator( int argc, char **argv )
		:	m_argv( argv )
		,	m_argc( argc )
	{
	}

	bool atEnd() const
	{
		return m_index == m_argc;
	}

	ArgumentIterator & operator++()
	{
		++m_index;
		return *this;
	}

	std::string operator*() const
	{
		return m_argv[ m_index ];
	}

private:
	char **m_argv;

	int m_argc;
	int m_index = 0;
};

}


//----------------------------------------------------------------
Arguments
parseArguments( int argc, char *argv[] )
{
	Arguments result;

	ArgumentIterator it( argc, argv );

	if( ( ++it ).atEnd() )
		return result;

	result.m_sqlFile = Util::makePath( *it );
	if( !result.m_sqlFile.has_extension() )
		result.m_sqlFile.replace_extension( std::filesystem::path( ".sql" ) );

	++it;

	while( !it.atEnd() )
	{
		if( Util::equalsNoCase( *it, "-o" ) )
		{
			if( ( ++it ).atEnd() )
				return Arguments();

			result.m_dbOutputFile = Util::makePath( *it );

			if( !result.m_dbOutputFile.has_extension() )
				result.m_dbOutputFile.replace_extension( std::filesystem::path( ".db3" ) );
		}
		else if( Util::equalsNoCase( *it, "-c" ) )
		{
			if( ( ++it ).atEnd() )
				return Arguments();

			result.m_generateCodeDirectory = Util::makePath( *it );
		}
		else
			return Arguments();

		++it;
	}

	return result;
}

