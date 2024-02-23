/*
 * db_schema_object_cache.hpp
 *
 *  Created on: 19 sty 2024
 *      Author: supcio
 */

#pragma once
#include "in_memory_sqlite_database.hpp"
#include "db_object_extract.hpp"
#include "db_objects.hpp"
#include <optional>
#include <set>

//using DbTriggersOfTable
//		= std::multiset< DbParsedTrigger, CompareByField< &DbParsedTrigger::m_moment > >;

using DbTriggersOfTable = std::vector< DbParsedTrigger >;

//=====================================================================
class DbSchemaObjectCache
{
public:
	DbSchemaObjectCache( InMemorySqliteDatabase const * _database );

	virtual ~DbSchemaObjectCache();

	std::optional< DbSchemaObject > getTable( std::string const & _table );

	bool isColumnIndexed( std::string const & _table, std::string const & _column );

	DbTriggersOfTable getTriggers( std::string const & _table );

private:
	struct TableIndexedColumns
	{
		std::string m_table;

		std::set< std::string, CompareNoCase > m_columns;
	};

private:
	void ensureTables();

	void ensureTriggers();

	void ensureTableIndexes( std::string const & _table );

private:
	InMemorySqliteDatabase const * m_database;

	std::optional< DbSchemaObjectSet > m_tables;

	std::optional< DbParsedTriggerSet > m_triggers;

	std::set< TableIndexedColumns, CompareByField< &TableIndexedColumns::m_table > > m_indexedTables;
};

