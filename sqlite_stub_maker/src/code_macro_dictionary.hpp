/*
 * code_macro_dictionary.hpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#pragma once
#include <string>
#include <functional>
#include <unordered_map>

//==============================================================================
class CodeMacroDictionary
{
public:
	using GetValueFunction = std::function< std::string() >;

public:
	CodeMacroDictionary() = default;

	CodeMacroDictionary( CodeMacroDictionary const * _next );

	void setMacro( std::string const & _macro, std::string const & _value );

	void setMacro( std::string const & _macro, GetValueFunction && _getValue );

	std::string getMacro( std::string const & _macro ) const;

private:
	std::unordered_map< std::string, GetValueFunction > m_mapMacroToGetValue;

	CodeMacroDictionary const * m_next = nullptr;
};

