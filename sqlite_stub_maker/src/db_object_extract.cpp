/*
 * db_object_extract.cpp
 *
 *  Created on: 17 sty 2024
 *      Author: supcio
 */
#include "db_object_extract.hpp"
#include "sql_analyzer.hpp"
#include "sql_tokenizer.hpp"
#include "util.hpp"
#include <boost/algorithm/string.hpp>
#include <sqlite3.h>

namespace
{

//------------------------------------------------------------------------------
std::string
getTableAlias(
		std::vector< SqlToken >::const_iterator _it
	,	std::vector< SqlToken >::const_iterator  _end
	)
{
	auto isKeyword = []( std::string const & _word )
	{
		return sqlite3_keyword_check( _word.c_str(), _word.size() );
	};

	if( ++_it == _end )
		return {};

	if( _it->m_kind == SqlTokenKind::Word && Util::equalsNoCase( _it->m_content, "AS" ) )
	{
		if( ++_it == _end )
			return {};
	}

	if( _it->m_kind == SqlTokenKind::Word && !isKeyword( _it->m_content ) )
		return _it->m_content;
	else
		return {};
}

}

//------------------------------------------------------------------------------
std::optional< DbParsedView >
dbExtractView(
		std::vector< SqlToken > const & _tokens
	, 	std::function< bool( std::string const & ) > const & _isKnownTable
	)
{
	SqlAnalyzer analyzer( _tokens );

	if( !analyzer.nextTokensLike( { "CREATE", "VIEW" } ) )
		return std::optional< DbParsedView >();

	analyzer.nextTokensLike( { "IF", "NOT", "EXISTS" } );

	DbParsedView result;
	result.m_name = *analyzer.nextIdentifier();

	if( !analyzer.findAndJumpAfter( { "SELECT" } ) )
		return std::optional< DbParsedView >();

	if( !analyzer.findAndJumpAfter( { "FROM" } ) )
		return std::optional< DbParsedView >();

	std::map< std::string, std::string, CompareNoCase > mapAliasToTable;

	for( auto it = analyzer.current(); it != _tokens.end(); ++it )
	{
		if( it->m_kind != SqlTokenKind::Word )
			continue;

		if( !_isKnownTable( it->m_content ) )
			continue;

		std::string table = it->m_content;
		mapAliasToTable[ table ] = {};

		result.m_sourceTables.insert( table );

		std::string alias = getTableAlias( it, _tokens.end() );

		if( !alias.empty() )
			mapAliasToTable[ alias ] = table;
	}

	if( mapAliasToTable.empty() )
		return std::optional< DbParsedView >();

	bool wasFrom = false;

	for( SqlToken const & token : _tokens )
	{
		if( token.m_kind != SqlTokenKind::Word )
			continue;

		if( Util::equalsNoCase( token.m_content, "FROM" ) )
			wasFrom = true;

		if( !wasFrom )
			continue;

		auto pos = token.m_content.find( '.' );
		if( pos == std::string::npos )
			continue;

		std::string tableOfColumn = token.m_content.substr( 0, pos );

		auto it = mapAliasToTable.find( tableOfColumn );
		if( it == mapAliasToTable.end() )
			continue;

		if( !it->second.empty() )
			tableOfColumn = it->second;

		std::string column = token.m_content.substr( pos + 1 );

		result.m_joinColumns.insert( { tableOfColumn, column } );
	}

	return result;
}

//-----------------------------------------------------------------------------
std::optional< DbParsedTrigger >
dbExtractTrigger( std::vector< SqlToken > const & _sqlCommand )
{
	SqlAnalyzer analyzer( _sqlCommand );

	if( !analyzer.nextTokensLike( { "CREATE", "TRIGGER" } ) )
		return std::optional< DbParsedTrigger >();

	analyzer.nextTokensLike( { "IF", "NOT", "EXISTS" } );

	DbParsedTrigger trigger;

	auto name = analyzer.nextIdentifier();
	if( !name )
		return std::optional< DbParsedTrigger >();

	trigger.m_name = *name;

	if( analyzer.nextTokensLike( { "BEFORE" } ) )
		trigger.m_moment = DbParsedTrigger::Moment::Before;
	else if( analyzer.nextTokensLike( { "AFTER" } ) )
		trigger.m_moment = DbParsedTrigger::Moment::After;
	else if( analyzer.nextTokensLike( { "INSTEAD", "OF" } ) )
		trigger.m_moment = DbParsedTrigger::Moment::InsteadOf;
	else
		return std::optional< DbParsedTrigger >();

	if( analyzer.nextTokensLike( { "INSERT" } ) )
		trigger.m_kind = DbParsedTrigger::Kind::Insert;
	else if( analyzer.nextTokensLike( { "DELETE" } ) )
		trigger.m_kind = DbParsedTrigger::Kind::Delete;
	else if( analyzer.nextTokensLike( { "UPDATE", "OF" } ) )
	{
		trigger.m_kind = DbParsedTrigger::Kind::Update;

		do
		{
			std::string column = *analyzer.nextIdentifier();

			trigger.m_updateColumns.insert( column );
		}
		while( analyzer.nextTokensLike( SqlTokenKind::Comma ) );
	}

	if( !analyzer.nextTokensLike( { "ON" } ) )
		return std::optional< DbParsedTrigger >();

	trigger.m_table = *analyzer.nextIdentifier();

	return trigger;

}

//-----------------------------------------------------------------------------
std::optional< DbParsedTrigger >
dbExtractTrigger( std::string const & _sqlCommand )
{
	SqlTokenizer tokenizer( _sqlCommand );

	std::vector< SqlToken > tokens;

	auto next = tokenizer.next();
	while( next )
	{
		tokens.push_back( *next );
		next = tokenizer.next();
	}

	return dbExtractTrigger( tokens );
}
