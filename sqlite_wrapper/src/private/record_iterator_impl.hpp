/*
 * record_iterator_impl.hpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */

#pragma once
#include "private/sqlite_handle.hpp"
#include "i_record_iterator.hpp"
#include <sqlite3.h>

namespace sqlw
{

//=================================================================================
class RecordIteratorImpl : public IRecordIterator
{
public:
	static std::shared_ptr< RecordIteratorImpl > create( SqliteHandle< sqlite3_stmt > const & _queryStmt );

	virtual ~RecordIteratorImpl();

	void step();

	bool hasNext() const override;

	void next( IRecordWrapper & _record ) override;

private:
	RecordIteratorImpl( SqliteHandle< sqlite3_stmt > const & _queryStmt );

private:
	SqliteHandle< sqlite3_stmt > m_queryStmt;

	int m_stepResult = 0;
};

} /* namespace sqlw */
