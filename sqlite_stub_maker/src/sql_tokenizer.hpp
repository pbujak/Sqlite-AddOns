/*
 * sql_tokenizer.hpp
 *
 *  Created on: 13 sty 2024
 *      Author: supcio
 */

#pragma once
#include "sql_string_iterator.hpp"
#include "sql_token.hpp"
#include <optional>

//=============================================================================
class SqlTokenizer
{
public:
	SqlTokenizer( std::string const & _sqlString );

	std::optional< SqlToken > next();

private:
	using NextTokenMethod = std::optional< SqlToken > ( SqlTokenizer::* )();

private:
	void skipSpaces();

	std::optional< SqlToken > nextWord();

	std::optional< SqlToken > nextHexadecimal();

	std::optional< SqlToken > nextNumeric();

	std::optional< SqlToken > nextLiteral();

	std::optional< SqlToken > nextVariable();

	std::optional< SqlToken > nextOperator();

	std::optional< SqlToken > nextSpecialCharacter();

	std::optional< SqlToken > nextCommentBlock();

private:
	SqlStringIterator m_itString;
};


