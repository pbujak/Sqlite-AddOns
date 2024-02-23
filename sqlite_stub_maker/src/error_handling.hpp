/*
 * ErrorHandling.hpp
 *
 *  Created on: 8 sty 2024
 *      Author: supcio
 */

#pragma once
#include <string>

//=========================================================================
class ErrorInfo
{
public:
	ErrorInfo( std::string const & _category, std::string const & _message );

	ErrorInfo& setLocation( int _line, int _column = -1 );

	ErrorInfo& setCode( int _code );

	std::string formatMessage( std::string const & _contextFile, std::string const & _errorOrWarning ) const;

private:
	std::string m_category;
	std::string m_message;

	int m_code = 0;
	int m_line = -1;
	int m_column = -1;
};

//=========================================================================
class ErrorHandling
{
public:
	[[ noreturn ]]
	static void throwError( ErrorInfo const & _errorInfo );

	static void reportWarning( ErrorInfo const & _errorInfo );

	static std::string setContextFile( std::string const & _file );

	static void printMessages();

private:
	ErrorHandling() = delete;
	~ErrorHandling() = delete;
};

//=========================================================================
class ScopedContextFile
{
public:
	ScopedContextFile( std::string const & _contextFile );

	~ScopedContextFile();

private:
	std::string m_oldContextFile;
};
