/*
 * in_memory_sqlite_database.hpp
 *
 *  Created on: 15 sty 2024
 *      Author: supcio
 */

#pragma once
#include "db_objects.hpp"
#include "db_auxiliary.hpp"
#include "sqlite_handle.hpp"
#include <string>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <sqlite3.h>
#include <functional>


//=================================================================
class InMemorySqliteDatabase
{
public:
	void execute(
			std::string const & _sql
		,	std::unordered_map< std::string, SqlValue > const & _variables
		,	std::function< void( sqlite3_stmt * const ) > const & _rowCallback
				= []( sqlite3_stmt * const ) {}
		);

	SqlValue executeWithResult(
			std::string const & _sql
		,	std::unordered_map< std::string, SqlValue > const & _variables
		);

	sqlite3_stmt *const getPreparedStatement( std::string const & _sql ) const;;

	DbSchemaObjectSet getTablesOrViews() const;

	DbSchemaObjectSet getTriggers() const;

	DbTableInfo getTableInfo( std::string const & _table ) const;

	DbIndexList getIndexList( std::string const & _table ) const;

	DbIndexInfo getIndexInfo( std::string const & _indexName ) const;

	DbForeignKeyList getForeignKeyList( std::string const & _table ) const;

	int getTableRowCount( std::string const & _table ) const;

	bool dump( std::filesystem::path const & _target, std::string & _errorMessage ) const;

	InMemorySqliteDatabase() = default;

	InMemorySqliteDatabase( InMemorySqliteDatabase const & ) = delete;

	InMemorySqliteDatabase( InMemorySqliteDatabase && ) = default;

private:
	using SqliteDb = SqliteHandle< sqlite3 >;

	using SqliteStatement = SqliteHandle< sqlite3_stmt >;

private:
	sqlite3 * const ensureConnection();

	template< typename _Container, typename _CreateItem, typename ... _BindParameters >
	void getItems(
			_Container & _container
		, 	std::string const & _sql
		,	_CreateItem const & _createItem
		, 	_BindParameters const & ... _parameters
		) const;

private:
	SqliteDb m_connection;

	std::unordered_map< std::string, SqliteStatement > mutable m_preparedStatementPool;
};

