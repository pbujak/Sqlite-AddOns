/*
 * util.cpp
 *
 *  Created on: 9 sty 2024
 *      Author: supcio
 */

#include "util.hpp"

namespace Util
{

std::filesystem::path
makePath( std::string _text )
{
	if( _text.substr( 0, 1 ) == "~" )
		_text.replace( 0, 1, getenv( "HOME" ) );

	return std::filesystem::path( _text );
}

int
compareNoCase( std::string const & _lhs, std::string const & _rhs )
{
	auto it1 = _lhs.begin();
	auto it2 = _rhs.begin();

	auto itEnd1 = _lhs.end();
	auto itEnd2 = _rhs.end();

	auto comp = []( char _ch1, char _ch2 ) -> int
	{
		if( _ch1 == _ch2 )
			return 0;

		return std::tolower( _ch1 ) - std::tolower( _ch2 );
	};

	for( ; it1 != itEnd1 && it2 != itEnd2; ++it1, ++it2 )
	{
		int result = comp( *it1, *it2 );
		if( result != 0 )
			return result;
	}

	if( it1 != itEnd1 )
		return +1;

	if( it2 != itEnd2 )
		return -1;

	return 0;
}

bool
equalsNoCase( std::string const & _lhs, std::string const & _rhs )
{
	if( _lhs.size() != _rhs.size() )
		return false;

	return std::equal(
			_lhs.begin()
		,	_lhs.end()
		,	_rhs.begin()
		,	_rhs.end()
		,	[]( char _ch1, char _ch2 )
			{
				if( _ch1 == _ch2 )
					return true;

				return std::tolower( _ch1 ) == std::tolower( _ch2 );
			}
		);
}

std::string
quote( std::string const & _string )
{
	return std::string( "\"" ) + _string + "\"";
}

}

