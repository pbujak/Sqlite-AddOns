/*
 * load_sql_script.hpp
 *
 *  Created on: 8 sty 2024
 *      Author: supcio
 */
#pragma once

#include <string>
#include <map>
#include <filesystem>

using LineOffsetAndLength = std::pair< int, int >;
using OrderedMapLineOffsetToLength = std::map< int, int >;

//==============================================================
struct SqlScriptData
{
	std::string m_content;

	OrderedMapLineOffsetToLength m_lines;

	std::filesystem::path m_xmlOrmMappings;
};

SqlScriptData loadSqlScript( std::filesystem::path const & _source );
