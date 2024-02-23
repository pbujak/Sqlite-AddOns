/*
 * split_sql_into_commands.cpp
 *
 *  Created on: 15 sty 2024
 *      Author: supcio
 */
#include "split_sql_into_commands.hpp"
#include "sql_analyzer.hpp"
#include "sql_tokenizer.hpp"
#include <vector>

namespace
{

//------------------------------------------------------------------------------
std::vector< SqlToken > &
operator<<( std::vector< SqlToken > & _target, std::vector< SqlToken > && _source )
{
	if( _source.empty() )
		return _target;

	if( _target.empty() )
		_target = std::move( _source );
	else
	{
		for( SqlToken & token : _source )
		{
			_target.push_back( std::move( token ) );
		}
		_source.clear();
	}

	return _target;
}

//------------------------------------------------------------------------------
std::vector< SqlToken >
getNextSqlCommand( SqlTokenizer & _sqlTokenizer )
{
	std::vector< SqlToken > result;

	while( auto token = _sqlTokenizer.next() )
	{
		if( token->m_kind == SqlTokenKind::CommentBlock )
			continue;

		result.push_back( *token );

		if( token->m_kind == SqlTokenKind::SemiColon )
			break;
	}

	return result;
}

//------------------------------------------------------------------------------
bool
commandLike( std::vector< SqlToken > const & _sqlTokens, std::vector< std::string > const & _firstWords )
{
	SqlAnalyzer analyzer( _sqlTokens );

	return analyzer.nextTokensLike(_firstWords );
}

}

//------------------------------------------------------------------------
SqlCommandOrderedSet
splitSqlIntoCommands( std::string const & _sql )
{
	SqlCommandOrderedSet result;

	auto pushSqlCommand = [ &result, &_sql ](
			std::vector< SqlToken > && _sqlTriggerTokens
		,	std::vector< SqlToken > && _sqlCommandTokens
		)
	{
		SqlCommand sqlCommand;
		sqlCommand.m_tokens << std::move( _sqlTriggerTokens );
		sqlCommand.m_tokens << std::move( _sqlCommandTokens );

		sqlCommand.m_offset = sqlCommand.m_tokens.begin()->m_offset;

		auto itLast = sqlCommand.m_tokens.rbegin();
		int end = itLast->m_offset + itLast->m_content.size();

		sqlCommand.m_content = _sql.substr( sqlCommand.m_offset, end - sqlCommand.m_offset );

		result.insert( std::move( sqlCommand ) );
	};

	int triggerLevel = 0;

	SqlTokenizer sqlTokenizer( _sql );

	std::vector< SqlToken > sqlTriggerTokens;
	std::vector< SqlToken > sqlCommandTokens = getNextSqlCommand( sqlTokenizer );

	while( !sqlCommandTokens.empty() )
	{
		if( commandLike( sqlCommandTokens, { "CREATE", "TRIGGER" } ) )
			++triggerLevel;
		else if( commandLike( sqlCommandTokens, { "END", ";" } ) )
		{
			if( triggerLevel > 0 )
				--triggerLevel;
		}

		if( triggerLevel == 0 )
			pushSqlCommand( std::move( sqlTriggerTokens ), std::move( sqlCommandTokens ) );
		else
			sqlTriggerTokens << std::move( sqlCommandTokens );

		sqlCommandTokens = getNextSqlCommand( sqlTokenizer );
	}

	if( triggerLevel > 0 )
		pushSqlCommand( std::move( sqlTriggerTokens ), {} );

	return result;
}
