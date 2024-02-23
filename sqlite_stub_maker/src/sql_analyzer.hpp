/*
 * sql_analyzer.hpp
 *
 *  Created on: 16 sty 2024
 *      Author: supcio
 */

#pragma once
#include "sql_token.hpp"
#include <optional>
#include <string>
#include <vector>
#include <functional>

//================================================================
class SqlAnalyzer
{
public:
	using FindCondition
		=	std::function<
				bool(
						std::vector< SqlToken >::const_iterator _it
					,	std::vector< SqlToken >::const_iterator _end
					)
			>;

public:
	SqlAnalyzer( std::vector< SqlToken > const & _tokens );

	bool nextTokensLike( std::vector< std::string > const & _expectedWords, bool _moveAfterIfOk = true );

	bool nextTokensLike( SqlTokenKind _tokenKind, bool _moveAfterIfOk = true );

	std::optional< std::string > nextIdentifier();

	std::optional< std::string > nextVariable();

	bool findAndJumpAfter( std::vector< std::string > const & _expectedWords );

	bool findAndJumpAfter( FindCondition const & _findCondition );

	std::vector< SqlToken > const * const getUnderlyingTokens() const;

	std::vector< SqlToken >::const_iterator current() const;

	bool atEnd() const;

private:
	std::vector< SqlToken > const & m_tokens;

	std::vector< SqlToken >::const_iterator m_it;
	std::vector< SqlToken >::const_iterator m_end;
};

