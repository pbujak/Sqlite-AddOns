/*
 * code_macro_dictionary.cpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#include "code_macro_dictionary.hpp"

//------------------------------------------------------------------------------
CodeMacroDictionary::CodeMacroDictionary( CodeMacroDictionary const * _next )
	:	m_next( _next )
{
}

//------------------------------------------------------------------------------
void
CodeMacroDictionary::setMacro( std::string const & _macro, std::string const & _value )
{
	setMacro(
			_macro
		, 	[ _value ](){ return _value; }
	);
}

//------------------------------------------------------------------------------
void
CodeMacroDictionary::setMacro( std::string const & _macro, GetValueFunction && _getValue )
{
	m_mapMacroToGetValue[ _macro ] = std::move( _getValue );
}

//------------------------------------------------------------------------------
std::string
CodeMacroDictionary::getMacro( std::string const & _macro ) const
{
	auto it = m_mapMacroToGetValue.find( _macro );

	if( it != m_mapMacroToGetValue.end() )
		return it->second();

	if( m_next )
		return m_next->getMacro( _macro );

	return {};
}

//------------------------------------------------------------------------------
