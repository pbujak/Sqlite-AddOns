/*
 * table_accessor_impl.hpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_table_accessor.hpp"
#include "i_record_wrapper.hpp"
#include "i_database.hpp"
#include "private/sqlite_handle.hpp"
#include "private/column_data_provider.hpp"

namespace sqlw
{

//========================================================================
class TableAccessorImpl : public ITableAccessor
{
public:
	TableAccessorImpl(
			SqliteHandle< sqlite3 > const & _db
		, 	std::string const & _tableName
		, 	bool _assumeView
		);

	virtual ~TableAccessorImpl();

	void insert( IRecordWrapper & _record, bool _reload );

	void update( IRecordWrapper& _record );

	void save( IRecordWrapper& _record, bool _reloadAfterInsert ) override;

	void remove( std::int64_t _nId ) override;

	bool find( std::int64_t _nId, IRecordWrapper& _record ) override;

	int size() override;

	IRecordIteratorHandle getAll() override;

	IRecordIteratorHandle findBy( std::string const & _columnName, std::type_info const * const _type, ... ) override;

private:
	bool reset( sqlite3_stmt *const a_stmt ) const;

	std::int64_t getLastInsertId();

	void prepareInsert( IRecordWrapper const & _record );

	void prepareUpdate( IRecordWrapper const & _record );

	sqlite3_stmt * const prepare( std::string const & _sql );

private:
	SqliteHandle< sqlite3 > m_db;

	SqliteHandle< sqlite3_stmt > m_stmtInsert;
	SqliteHandle< sqlite3_stmt > m_stmtUpdate;

	std::unordered_map< std::string, SqliteHandle< sqlite3_stmt > > m_preparedStmtMap;

	std::string m_tableName;

	bool m_view = false;
};

} /* namespace sqlw */
