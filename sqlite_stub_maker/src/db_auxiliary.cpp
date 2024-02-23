/*
 * db_auxiliary.cpp
 *
 *  Created on: 26 sty 2024
 *      Author: supcio
 */

#include "db_auxiliary.hpp"

//------------------------------------------------------------------------------
SqlValue::SqlValue( sqlite3_value * const _value )
	:	m_value( sqlite3_value_dup( _value ), sqlite3_value_free )
{
}

//------------------------------------------------------------------------------
bool
SqlValue::empty() const
{
	return !m_value.get();
}

//------------------------------------------------------------------------------
template<>
std::optional< int >
SqlValue::get() const
{
	if( sqlite3_value_type( m_value.get() ) != SQLITE_INTEGER )
		return std::optional< int >();

	return sqlite3_value_int( m_value.get() );
}

//------------------------------------------------------------------------------
template<>
std::optional< double >
SqlValue::get() const
{
	if( sqlite3_value_type( m_value.get() ) != SQLITE_FLOAT )
		return std::optional< double >();

	return sqlite3_value_double( m_value.get() );
}

//------------------------------------------------------------------------------
template<>
std::optional< std::string >
SqlValue::get() const
{
	if( sqlite3_value_type( m_value.get() ) != SQLITE_TEXT )
		return std::optional< std::string >();

	return reinterpret_cast< char const* >( sqlite3_value_text( m_value.get() ) );
}

//------------------------------------------------------------------------------
SqlValue::operator sqlite3_value const * () const
{
	return m_value.get();
};

//------------------------------------------------------------------------------
SqlException::SqlException( std::string const & _message, int _code )
	:	m_message( _message )
	,	m_code( _code )
{
}

//------------------------------------------------------------------------------
std::string
SqlException::getMessage() const
{
	return m_message;
}

//------------------------------------------------------------------------------
int
SqlException::getCode() const
{
	return m_code;
}
