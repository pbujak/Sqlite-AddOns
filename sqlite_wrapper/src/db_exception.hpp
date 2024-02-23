/*
 * db_exception.hpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#pragma once
#include "dll_interface.hpp"
#include <string>

namespace sqlw
{

//==========================================================
class DLL_EXPORT DbException
{
public:
	enum class Category
	{
			Template
		,	Sqlite
		,	LogicError
	};
public:
	DbException( std::string const & _message, Category _category, int _code, int _extendedCode );

	~DbException();

	int getCode() const;

	int getExtendedCode() const;

	std::string getErrorMessage() const;

private:
	std::string m_message;

	Category m_category;

	int m_code;

	int m_extendedCode;
};

} /* namespace sqlw */
