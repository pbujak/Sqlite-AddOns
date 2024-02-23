/*
 * load_sql_script.cpp
 *
 *  Created on: 8 sty 2024
 *      Author: supcio
 */
#include "load_sql_script.hpp"
#include "error_handling.hpp"
#include "util.hpp"
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace
{

//---------------------------------------------------------------------------
void
removeLineEnding( std::string & _line )
{
	while(
			!_line.empty()
		&& 	( *_line.rbegin() == '\n' || *_line.rbegin() == '\r' )
		)
	{
		_line.pop_back();
	}
}

//---------------------------------------------------------------------------
bool
removeStartIfMatching( std::string & _string, std::string const & _pattern )
{
	int len = _pattern.size();

	if( !Util::equalsNoCase( _string.substr( 0, len ), _pattern ) )
		return false;

	_string.erase( 0, len );
	return true;
}

//---------------------------------------------------------------------------
std::filesystem::path
getOrmMappingFile(
		std::filesystem::path const & _sqlFile
	,	std::string const & _sql
	, 	OrderedMapLineOffsetToLength const & _lines
	)
{
	std::string line;
	int lineNumber = 1;

	for( auto const & offsetAndLength : _lines )
	{
		line = _sql.substr( offsetAndLength.first, offsetAndLength.second );
		boost::trim( line );

		if( !line.empty() )
			break;

		++lineNumber;
	}

	if( line.empty() )
		return std::filesystem::path();

	boost::trim( line );

	if( !removeStartIfMatching( line, "--#include-orm-mappings" ) )
		return std::filesystem::path();

	if( line.empty() )
	{
		ErrorHandling::throwError(
			ErrorInfo( "ORM", "Missing file definition in --#include-orm-mappings." )
				.setLocation( lineNumber )
		);
	}

	if( !std::isspace( *line.begin() ) )
	{
		ErrorHandling::throwError(
			ErrorInfo( "ORM", "Syntax error --#include-orm-mappings." )
				.setLocation( lineNumber )
		);
	}

	boost::trim_left( line );

	char constexpr DoubleQuote = '\"';
	char constexpr SingleQuote = '\'';

	bool quoteStart = *line.begin() == DoubleQuote;
	bool quoteEnd = *line.rbegin() == DoubleQuote;

	if( !quoteStart && !quoteEnd )
	{
		quoteStart = *line.begin() == SingleQuote;
		quoteEnd = *line.rbegin() == SingleQuote;
	}

	if( quoteStart != quoteEnd )
	{
		ErrorHandling::throwError(
			ErrorInfo( "ORM", "Syntax error \" in argument for --#include-orm-mappings." )
				.setLocation( lineNumber )
		);
	}

	if( quoteStart )
	{
		line.erase( 0, 1 );
		line.pop_back();
	}

	std::filesystem::path result = Util::makePath( line );

	if( result.is_relative() )
		result = _sqlFile.parent_path() / result;

	if( !std::filesystem::exists( result ) )
	{
		ErrorHandling::throwError(
			ErrorInfo( "FATAL", std::string( "ORM file not found " ) + result.string() )
				.setLocation( lineNumber )
		);
	}

	return result;
}

}

//---------------------------------------------------------------------------
SqlScriptData
loadSqlScript( std::filesystem::path const & _source )
{
	if( !std::filesystem::exists( _source ) )
	{
		ErrorHandling::throwError(
			ErrorInfo( "FATAL", std::string( "SQL file not found " ) + _source.string() )
		);
	}

	SqlScriptData result;

	std::ifstream file( _source.string() );

	std::string line;

	while( std::getline( file, line ) )
	{
		removeLineEnding( line );

		if( !result.m_content.empty() )
			result.m_content += "\n";

		result.m_lines.insert(
			std::make_pair( result.m_content.size(), line.size() )
			);

		result.m_content += line;
	}

	result.m_xmlOrmMappings = getOrmMappingFile( _source, result.m_content, result.m_lines );

	return result;
}


