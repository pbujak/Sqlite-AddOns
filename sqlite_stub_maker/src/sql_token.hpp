/*
 * sql_token.hpp
 *
 *  Created on: 13 sty 2024
 *      Author: supcio
 */

#pragma once
#include <string>

//================================================
enum class SqlTokenKind
{
		Unparsed
	,	Word
	,	Literal
	,	Numeric
	,	Variable
	,	Operator
	,	Comma
	,	SemiColon
	,	Bracket
	,	CommentBlock
};

//================================================
struct SqlToken
{
	SqlTokenKind m_kind;

	int m_offset;

	std::string m_content;
};


