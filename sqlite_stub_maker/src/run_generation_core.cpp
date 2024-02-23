/*
 * run_generation.cpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */
#include <boost/algorithm/string.hpp>
#include "run_generation_core.hpp"

namespace
{

//------------------------------------------------------------------------------
std::optional< std::string >
extractForEach( std::string const & _string )
{
	static std::string const ForEach = "[%FOR-EACH%]";

	auto pos = _string.find( ForEach );
	if( pos == std::string::npos )
		return std::optional< std::string >();

	pos += ForEach.size();

	pos = _string.find_first_not_of( " \t", pos );
	if( pos == std::string::npos )
		return std::optional< std::string >();

	if( _string[ pos ] != '(' )
		return std::optional< std::string >();

	int start = pos + 1;

	auto end = _string.find_first_of( ')', start );
	if( end == std::string::npos )
		return std::optional< std::string >();

	std::string substr = _string.substr( start, end - start );
	boost::trim( substr );

	return substr;
}

//------------------------------------------------------------------------------
void
replaceMacros( std::string & _string, CodeMacroDictionary const & _dictionary )
{
	static std::string const OpenTag = "[$";
	static std::string const CloseTag = "$]";

	auto openTagPos = _string.find( OpenTag );

	if( openTagPos == std::string::npos )
		return;

	int closeTagPos = _string.find( CloseTag, openTagPos );

	std::string key = _string.substr( openTagPos + 2, closeTagPos - openTagPos - 2 );

	_string.replace( openTagPos, closeTagPos + 2 - openTagPos, _dictionary.getMacro( key ) );

	replaceMacros( _string, _dictionary );
}


}

//------------------------------------------------------------------------------
void
runGenerationCore(
		StringVectorIterator _it
	,	StringVectorIterator _end
	,	CodeMacroDictionary const & _dictionary
	,	std::unordered_map< std::string, ForEachFunction > const & _forEachHandlerMap
	,	std::ostream & _target
	)
{
	auto getForEachHandler = [ &_forEachHandlerMap ]( std::string const & _key ) -> ForEachFunction
	{
		auto it = _forEachHandlerMap.find( _key );
		if( it == _forEachHandlerMap.end() )
			return ForEachFunction();

		return it->second;
	};

	auto findEndForEach = [ _end ]( auto _it2 )
	{
		std::string const End = "[%END%]";

		for( ; _it2 != _end; ++_it2 )
		{
			if( _it2->find( End ) != std::string::npos )
				break;
		}
		return _it2;
	};

	while( _it != _end )
	{
		std::string const & line = *_it;

		auto forEach = extractForEach( line );
		if( forEach )
		{
			auto begin2 = _it + 1;
			auto end2 = findEndForEach( begin2 );

			auto forEachHandler = getForEachHandler( *forEach );
			if( forEachHandler )
				forEachHandler( begin2, end2 );

			_it = end2 + 1;
			continue;
		}

		std::string newLine = line;
		replaceMacros( newLine, _dictionary );

		_target << newLine << std::endl;

		++_it;
	}
}
