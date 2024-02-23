/*
 * db_exception.cpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#include "db_exception.hpp"

namespace sqlw
{

//-----------------------------------------------------------------------------------------
DbException::DbException( std::string const & _message, Category _category, int _code, int _extendedCode )
	:	m_message( _message )
	,	m_category( _category )
	,	m_code( _code )
	,	m_extendedCode( _extendedCode )
{
}

//-----------------------------------------------------------------------------------------
int
DbException::getCode() const
{
	return m_code;
}

//-----------------------------------------------------------------------------------------
int
DbException::getExtendedCode() const
{
	return m_extendedCode;
}

//-----------------------------------------------------------------------------------------
std::string
DbException::getErrorMessage() const
{
	return m_message;
}

//-----------------------------------------------------------------------------------------
DbException::~DbException()
{
}

} /* namespace sqlw */
