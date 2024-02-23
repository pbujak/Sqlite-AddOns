/*
 * db_query.hpp
 *
 *  Created on: 15 lut 2024
 *      Author: supcio
 */

#pragma once
#include "db_record_iterator.hpp"
#include "i_database.hpp"
#include "i_query.hpp"

namespace sqlw
{

//===================================================================================
template< class _Record >
class DbQuery
{
public:
	DbQuery( IDatabaseHandle const & _database, std::string const & _sqlWhereClause );

	template< typename _Parameter >
	DbQuery< _Record > &
	bindParameter( _Parameter const & _parameter )
	{
		m_queryHandle->bindParameter( _parameter, {} );
		return *this;
	}

	template< typename _Parameter >
	DbQuery< _Record > &
	bindParameter( std::string const & _name, _Parameter const & _parameter )
	{
		m_queryHandle->bindParameter(_parameter, _name );
		return *this;
	}

	DbQuery< _Record > & reset();

	DbRecordIterator< _Record > getAll() const;

private:
	IDatabaseHandle m_database;

	IQueryHandle m_queryHandle;
};

//---------------------------------------------------------------------------
template< class _Record >
DbQuery< _Record >::DbQuery( IDatabaseHandle const & _database, std::string const & _sqlWhereClause )
	:	m_database( _database )
	,	m_queryHandle(
			_database->openQuery(
					std::string( _Record::Bindings::TableName.begin(), _Record::Bindings::TableName.end() )
				,	_sqlWhereClause
				)
			)
{
}

//---------------------------------------------------------------------------
template< class _Record >
DbRecordIterator< _Record >
DbQuery< _Record >::getAll() const
{
	return DbRecordIterator< _Record >( m_queryHandle->getAll() );
}

//---------------------------------------------------------------------------
template< class _Record >
DbQuery< _Record > &
DbQuery< _Record >::reset()
{
	m_queryHandle->reset();
	return *this;
}

}


