/*
 * column_data_provider.hpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */
#pragma once
#include "i_record_wrapper.hpp"
#include <string>
#include <unordered_map>
#include <sqlite3.h>

namespace sqlw
{

//==============================================================
class ColumnDataProvider
{
public:
	ColumnDataProvider();

	virtual ~ColumnDataProvider();

	void load( sqlite3_stmt * const a_stmt, IRecordWrapper & _record );

private:
	int getColumnId( sqlite3_stmt * const a_stmt, std::string const & _pszColName );

private:
	std::unordered_map< std::string, int > m_mapColumns;
};

} /* namespace sqlw */
