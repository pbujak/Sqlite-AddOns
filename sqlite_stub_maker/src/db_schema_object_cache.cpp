/*
 * db_schema_object_cache.cpp
 *
 *  Created on: 19 sty 2024
 *      Author: supcio
 */

#include "db_schema_object_cache.hpp"

//------------------------------------------------------------------------------
DbSchemaObjectCache::DbSchemaObjectCache( InMemorySqliteDatabase const* _database )
	:	m_database( _database )
{
}

//------------------------------------------------------------------------------
DbSchemaObjectCache::~DbSchemaObjectCache()
{
	// TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------
std::optional< DbSchemaObject >
DbSchemaObjectCache::getTable( std::string const& _table )
{
	ensureTables();

	auto it = m_tables->find( _table );
	if( it == m_tables->end() )
		return std::optional< DbSchemaObject >();

	return *it;
}

//------------------------------------------------------------------------------
void
DbSchemaObjectCache::ensureTables()
{
	if( !m_tables )
		m_tables = m_database->getTablesOrViews();
}

//------------------------------------------------------------------------------
void
DbSchemaObjectCache::ensureTableIndexes( std::string const & _table )
{
	auto it = m_indexedTables.find( _table );
	if( it != m_indexedTables.end() )
		return;

	TableIndexedColumns tic;
	tic.m_table = _table;

	DbTableInfo tableInfo = m_database->getTableInfo( _table );

	for( auto const & column : tableInfo )
	{
		if( column.m_primaryKey )
		{
			tic.m_columns.insert( column.m_name );
			break;
		}
	}

	DbIndexList indexList = m_database->getIndexList( _table );

	for( auto const & index : indexList )
	{
		DbIndexInfo indexInfo = m_database->getIndexInfo( index.m_name );

		for( auto const & indexPart : indexInfo )
		{
			if( indexPart.m_columnName )
				tic.m_columns.insert( *indexPart.m_columnName );
		}
	}

	m_indexedTables.insert( std::move( tic ) );
}

//------------------------------------------------------------------------------
bool
DbSchemaObjectCache::isColumnIndexed( std::string const & _table, std::string const & _column )
{
	ensureTableIndexes( _table );

	auto it = m_indexedTables.find( _table );
	if( it == m_indexedTables.end() )
		return false;

	return it->m_columns.find( _column ) != it->m_columns.end();
}

//------------------------------------------------------------------------------
void
DbSchemaObjectCache::ensureTriggers()
{
	if( m_triggers )
		return;

	m_triggers.emplace();

	auto rawTriggers = m_database->getTriggers();

	for( DbSchemaObject const & raw : rawTriggers )
	{
		auto trigger = dbExtractTrigger( raw.m_sql );
		if( trigger )
			m_triggers->insert( *trigger );
	}
}

//------------------------------------------------------------------------------
DbTriggersOfTable
DbSchemaObjectCache::getTriggers( std::string const & _table )
{
	ensureTriggers();

	DbTriggersOfTable result;

	auto eqr = m_triggers->equal_range( _table );

	for( auto it = eqr.first; it != eqr.second; ++it )
	{
		result.push_back( *it );
	}

	return result;
}
