/*
 * run_generation.hpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#pragma once
#include "code_macro_dictionary.hpp"
#include <vector>


using StringVector = std::vector< std::string >;

using StringVectorIterator = StringVector::const_iterator;

using ForEachFunction = std::function< void( StringVectorIterator _it, StringVectorIterator _end ) >;


//------------------------------------------------------------------------------
void
runGenerationCore(
		StringVectorIterator _it
	,	StringVectorIterator _end
	,	CodeMacroDictionary const & _dictionary
	,	std::unordered_map< std::string, ForEachFunction > const & _forEachHandlerMap
	,	std::ostream & _target
	);

