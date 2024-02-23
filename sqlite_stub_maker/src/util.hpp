/*
 * util.hpp
 *
 *  Created on: 9 sty 2024
 *      Author: supcio
 */
#include <string>
#include <filesystem>
#include <sstream>

#pragma once

namespace Util
{

std::filesystem::path makePath( std::string _text );

bool equalsNoCase( std::string const & _lhs, std::string const & _rhs );

int compareNoCase( std::string const & _lhs, std::string const & _rhs );

std::string quote( std::string const & _string );

template< typename ... _FragmentN >
std::string formatMessage( _FragmentN const & ... _fragments );

}

namespace detail
{

//------------------------------------------------------------------------------
template< typename _Fragment1, typename ... _FragmentN >
inline void
formatMessageImpl(
		std::stringstream & _ss
	, 	_Fragment1 const & _fragment1
	, 	_FragmentN const & ... _fragmentN
	)
{
	_ss << _fragment1;

	if constexpr ( sizeof ...( _fragmentN ) > 0 )
		formatMessageImpl( _ss, _fragmentN ... );
}

}

//-------------------------------------------------------------------------------
template< typename ... _FragmentN >
std::string
Util::formatMessage( _FragmentN const & ... _fragments )
{
	std::stringstream ss;
	detail::formatMessageImpl( ss, _fragments ... );
	return ss.str();
}
