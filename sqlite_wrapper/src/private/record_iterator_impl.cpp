/*
 * record_iterator_impl.cpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */

#include "private/record_iterator_impl.hpp"
#include "private/column_data_provider.hpp"

namespace sqlw
{

//-----------------------------------------------------------------------------------
RecordIteratorImpl::RecordIteratorImpl( SqliteHandle< sqlite3_stmt > const & _queryStmt )
	:	m_queryStmt( _queryStmt )
{
}

//-----------------------------------------------------------------------------------
RecordIteratorImpl::~RecordIteratorImpl()
{
}

//-----------------------------------------------------------------------------------
std::shared_ptr< RecordIteratorImpl >
RecordIteratorImpl::create( SqliteHandle<sqlite3_stmt> const & _queryStmt )
{
	std::shared_ptr< RecordIteratorImpl > result( new RecordIteratorImpl(_queryStmt ) );

	result->step();

	return result;
}

//-----------------------------------------------------------------------------------
void
RecordIteratorImpl::step()
{
	m_stepResult = sqlite3_step( m_queryStmt );
}

//-----------------------------------------------------------------------------------
bool
RecordIteratorImpl::hasNext() const
{
	return m_stepResult == SQLITE_ROW;
}

//-----------------------------------------------------------------------------------
void
RecordIteratorImpl::next( IRecordWrapper& _record )
{
	ColumnDataProvider columnDataProvider;

	columnDataProvider.load( m_queryStmt, _record );

	step();
}

} /* namespace sqlw */


