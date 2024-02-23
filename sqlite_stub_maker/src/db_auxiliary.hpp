/*
 * db_auxiliary.hpp
 *
 *  Created on: 26 sty 2024
 *      Author: supcio
 */

#pragma once
#include "sql_token.hpp"
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <sqlite3.h>
#include <vector>

//=============================================================
struct SqlCommand
{
	int m_offset;

	std::string m_content;

	std::vector< SqlToken > m_tokens;

	inline bool operator<( SqlCommand const & _rhs ) const
	{
		return m_offset < _rhs.m_offset;
	}
};

using SqlCommandOrderedSet = std::set< SqlCommand >;

//==============================================================================
class SqlValue
{
public:
	SqlValue() = default;

	SqlValue( sqlite3_value * const _value );

	bool empty() const;

	template< typename _T >
	std::optional< _T > get() const;

	operator sqlite3_value const * () const;

private:
	std::shared_ptr< sqlite3_value > m_value;
};

//==============================================================================
class SqlException
{
public:
	SqlException( std::string const & _message, int _code = SQLITE_ERROR );

	std::string getMessage() const;

	int getCode() const;

private:
	std::string m_message;

	int m_code;
};
