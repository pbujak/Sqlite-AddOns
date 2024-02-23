/*
 * sql_engine.cpp
 *
 *  Created on: 26 sty 2024
 *      Author: supcio
 */

#include "sql_engine.hpp"
#include "sql_analyzer.hpp"
#include "execute_assertion.hpp"
#include "util.hpp"
#include <boost/algorithm/string.hpp>

namespace
{

//------------------------------------------------------------------------------
std::string
getSelectCommandForView( std::vector< SqlToken > const & _sqlCommand )
{
	std::string result;

	bool selectClause = false;

	for( SqlToken const & token : _sqlCommand )
	{
		if( selectClause )
		{
			result += token.m_content + " ";
		}

		if( Util::equalsNoCase( token.m_content, "AS" ) )
		{
			selectClause = true;
		}
	}
	return result;
}

//------------------------------------------------------------------------
std::vector< std::string >
addHelpersSqlCommands( SqlCommand const & _sqlCommand )
{
	std::vector< std::string > commands;

	{
		SqlAnalyzer analyzer( _sqlCommand.m_tokens );

		if( analyzer.nextTokensLike( { "CREATE", "VIEW" } ) )
			commands.push_back( getSelectCommandForView( _sqlCommand.m_tokens ) );
	}

	commands.push_back( _sqlCommand.m_content );

	return commands;
}

}

//-----------------------------------------------------------------------------
SqlEngine::SqlEngine( InMemorySqliteDatabase * _database )
	:	m_database( _database )
{
}

//-----------------------------------------------------------------------------
SqlEngine::~SqlEngine()
{
}

//-----------------------------------------------------------------------------
void
SqlEngine::execute( SqlCommand const & _sqlCommand )
{
	std::unordered_map< std::string, SqlValue > parameters = extractParameters( _sqlCommand );

	if( executeAssertion( *m_database, _sqlCommand, parameters ) )
		return;

	if( executeAssignment( _sqlCommand, parameters ) )
		return;

	auto allSqlCommands = addHelpersSqlCommands( _sqlCommand );

	for( std::string const & sql : allSqlCommands )
	{
		m_database->execute( sql, parameters );
	}
}

//-----------------------------------------------------------------------------
std::unordered_map< std::string, SqlValue >
SqlEngine::extractParameters( SqlCommand const & _sqlCommand )
{
	std::unordered_map< std::string, SqlValue > parameters;

	auto itToken = _sqlCommand.m_tokens.begin();

	if( Util::equalsNoCase( itToken->m_content, "SET" ) )
	{
		for( int i = 0; i < 2; ++i )
		{
			if( itToken != _sqlCommand.m_tokens.end() )
				++itToken;
		}
	}

	for( ; itToken != _sqlCommand.m_tokens.end(); ++itToken )
	{
		if( itToken->m_kind != SqlTokenKind::Variable )
			continue;

		std::string lower = boost::to_lower_copy( itToken->m_content );

		auto it = m_variableDictionary.find( lower );
		if( it == m_variableDictionary.end() )
			throw SqlException( "Undefined variable " + Util::quote( itToken->m_content ) + "." );

		parameters[ itToken->m_content ] = it->second;
	}

	return parameters;
}

//-----------------------------------------------------------------------------
bool
SqlEngine::executeAssignment(
		SqlCommand const & _sqlCommand
	,   std::unordered_map< std::string, SqlValue > const & _parameters
	)
{
	SqlAnalyzer analyzer( _sqlCommand.m_tokens );

	if( !analyzer.nextTokensLike( { "SET" } ) )
		return false;

	auto assignVariable = analyzer.nextVariable();
	if( !assignVariable )
		return false;

	if( !analyzer.nextTokensLike( { "=" } ) )
		return false;

	if( analyzer.atEnd() )
		return false;

	int offsetDiff = analyzer.current()->m_offset - _sqlCommand.m_tokens[ 0 ].m_offset;

	std::string firstWord = analyzer.current()->m_content;

	std::string sql = _sqlCommand.m_content.substr( offsetDiff );

	if( !Util::equalsNoCase( firstWord, "SELECT" ) )
		sql.insert( 0, "SELECT " );

	SqlValue result = m_database->executeWithResult( sql, _parameters );

	m_variableDictionary[ boost::to_lower_copy( *assignVariable ) ] = result;

	return true;
}
