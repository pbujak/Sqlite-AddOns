/*
 * execute_assertion.cpp
 *
 *  Created on: 25 sty 2024
 *      Author: supcio
 */
#include "execute_assertion.hpp"
#include "error_handling.hpp"
#include "util.hpp"
#include <sstream>
#include <string_view>

//------------------------------------------------------------------------------
bool
executeAssertion(
		InMemorySqliteDatabase & _inMemoryDatabase
	, 	SqlCommand const & _sqlCommand
	, 	std::unordered_map< std::string, SqlValue > const & _parameters
	)
{
	auto token0 = _sqlCommand.m_tokens[ 0 ];

	if(
			token0.m_kind != SqlTokenKind::Word
		|| 	!Util::equalsNoCase( token0.m_content, "ASSERT" )
		)
	{
		return false;
	}

	std::string_view constexpr Prefix = "SELECT CASE WHEN ( ";
	std::string_view constexpr Suffix = " ) THEN 1 ELSE 0 END;";

	std::string sql;
	sql += Prefix;

	int bracketLevel = 0;

	for( auto it = _sqlCommand.m_tokens.begin() + 1; it != _sqlCommand.m_tokens.end(); ++it )
	{
		if( it->m_kind == SqlTokenKind::SemiColon )
			break;

		if( it->m_kind == SqlTokenKind::Bracket )
		{
			if( it->m_content == "(" )
				++bracketLevel;
			else if( it->m_content == ")" )
				--bracketLevel;
		}

		sql += it->m_content + " ";
	}

	if( bracketLevel != 0 )
		throw SqlException( "Syntax error ()." );

	sql += Suffix;

	int result = 0;

	auto getResult = [ &result ]( sqlite3_stmt* const _stmt )
		{
			result = sqlite3_column_int( _stmt, 0 );
		};


	_inMemoryDatabase.execute( sql, _parameters, getResult );

	if( result != 1 )
		throw SqlException( "Assertion failed " );

	return true;
}


