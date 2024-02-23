/*
 * ErrorHandling.cpp
 *
 *  Created on: 8 sty 2024
 *      Author: supcio
 */

#include "error_handling.hpp"
#include "config.hpp"
#include <exception>
#include <utility>
#include <vector>
#include <sstream>
#include <iostream>

namespace
{

std::vector< std::string > s_warnings;
std::vector< std::string > s_errors;

std::string s_contextFile;

}

//--------------------------------------------------------
void
ErrorHandling::throwError( ErrorInfo const & _errorInfo )
{
	s_errors.push_back( _errorInfo.formatMessage( s_contextFile, "error" ) );

	throw std::exception();
}

//--------------------------------------------------------
void
ErrorHandling::reportWarning( ErrorInfo const & _errorInfo )
{
	s_warnings.push_back( _errorInfo.formatMessage( s_contextFile, "warning" ) );
}

//--------------------------------------------------------
std::string
ErrorHandling::setContextFile( std::string const & _file )
{
	return std::exchange( s_contextFile, _file );
}

//--------------------------------------------------------
void
ErrorHandling::printMessages()
{
	if constexpr( Platform == PlatformType::Linux )
		std::cout << "\033[33m"; // bold yellow

	for( std::string const & warning : s_warnings )
		std::cout << warning << std::endl;


	if constexpr( Platform == PlatformType::Linux )
		std::cout << "\033[1m\033[31m"; // bold red

	for( std::string const & error : s_errors )
		std::cout << error << std::endl;


	if constexpr( Platform == PlatformType::Linux )
		std::cout << "\033[0m"; // reset

	std::cout << std::endl;
}

//--------------------------------------------------------
ErrorInfo::ErrorInfo( std::string const & _category, std::string const & _message )
	:	m_category( _category )
	,	m_message( _message )
{
}

//--------------------------------------------------------
ErrorInfo&
ErrorInfo::setLocation( int _line, int _column )
{
	m_line = _line;
	m_column = _column;
	return *this;
}

//--------------------------------------------------------
ErrorInfo&
ErrorInfo::setCode( int _code )
{
	m_code = _code;
	return *this;
}

//--------------------------------------------------------
std::string
ErrorInfo::formatMessage( std::string const & _contextFile, std::string const & _errorOrWarning ) const
{
	std::stringstream ss;

	ss << _contextFile;

	if( m_line != -1 )
	{
		ss << '(' << m_line;

		if( m_column != -1 )
			ss << ',' << m_column;

		ss << ')';
	}

	ss << ": " << m_category << " " << _errorOrWarning;

	if( m_code > 0 )
		ss << " " << m_code;
	ss << ": " << m_message;

	return ss.str();
}

//--------------------------------------------------------
ScopedContextFile::ScopedContextFile( std::string const & _contextFile )
{
	m_oldContextFile = ErrorHandling::setContextFile( _contextFile );
}

//--------------------------------------------------------
ScopedContextFile::~ScopedContextFile()
{
	ErrorHandling::setContextFile( m_oldContextFile );
}
