/*
 * db_objects.hpp
 *
 *  Created on: 17 sty 2024
 *      Author: supcio
 */

#pragma once
#include "string_key.hpp"
#include <string>
#include <unordered_set>
#include <set>
#include <map>
#include <optional>
#include <variant>

//==============================================================================
struct DbSchemaObject
{
	enum class Kind
	{
			Unknown
		,	Table
		,	View
		,	Trigger
		,	Index
	};

	Kind m_kind = Kind::Unknown;

	std::string m_name;
	std::string m_relatedTable;

	int m_rootPage = 0;

	std::string m_sql;
};

using DbSchemaObjectSet = std::set< DbSchemaObject, CompareByField< &DbSchemaObject::m_name > >;

//========================================================
struct DbTableColumn
{
	std::string m_name;
	std::string m_type;
	bool        m_notNull = false;
	std::string m_default;
	bool        m_primaryKey;
};

using DbTableInfo = std::vector< DbTableColumn >;

//==============================================================================
struct DbForeignKey
{
	std::string m_tableForeign;
	std::string m_columnForeign;
	std::string m_columnLocal;
};

using DbForeignKeyList = std::vector< DbForeignKey >;

//==============================================================================
struct DbIndex
{
	std::string m_name;

	bool m_unique;

	std::string m_kind;

	bool m_partial;
};

using DbIndexList = std::vector< DbIndex >;

//==============================================================================
struct DbIndexedColumn
{
	int m_columnRankWithinIndex;
	int m_columnRankWithinTable;

	std::optional< std::string > m_columnName;
};

using DbIndexInfo = std::vector< DbIndexedColumn >;

//==============================================================================
struct DbParsedView
{
	std::string m_name;

	std::set< std::string, CompareNoCase > m_sourceTables;

	std::multimap< std::string, std::string, CompareNoCase > m_joinColumns;
};

//========================================================
struct DbParsedTrigger
{
	enum class Kind
	{
			Insert
		,	Delete
		,	Update
	};

	enum class Moment
	{
			Before
		,	After
		,	InsteadOf
	};

	std::string m_name;
	std::string m_table;

	Kind m_kind;

	Moment m_moment;

	std::set< std::string, CompareNoCase > m_updateColumns;
};

//========================================================
using DbParsedTriggerSet
		= std::multiset< DbParsedTrigger, CompareByField< &DbParsedTrigger::m_table > >;

//========================================================
using DbReturnValue = std::variant< int, double, std::string >;
