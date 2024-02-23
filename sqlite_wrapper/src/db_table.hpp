/*
 * db_table.hpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */

#pragma once
#include "db_table_auxiliary.hpp"
#include "db_record_iterator.hpp"
#include "i_database.hpp"
#include <optional>
#include <type_traits>

namespace sqlw
{

//===================================================================================
template< class _Record >
class DbTable
{
public:
	DbTable( IDatabaseHandle const & _database );

	int size() const;

	std::optional< _Record > find( std::int64_t _id ) const;

	template< auto _FieldPointer, typename _Value >
	DbRecordIterator< _Record > findBy( _Value const & _value )
	{
		return findByImpl( MakeIntegralConstant< _FieldPointer >(), _value );
	}

	DbRecordIterator< _Record > getAll() const;

	std::enable_if_t< _Record::Bindings::IsModifiable >
	save( _Record & _record )
	{
		m_tableHandle->save( *getRecordWraper( &_record ), true );
	}

	std::enable_if_t< _Record::Bindings::IsModifiable >
	save( _Record && _record )
	{
		_Record record2 = std::move( _record );
		_record.m_id = -1;
		m_tableHandle->save( *getRecordWraper( &record2 ), false );
	}

	void remove( std::int16_t _id );

private:
	template<
			typename _FieldType
		,	_FieldType ( _Record::*_Member )
		>
	DbRecordIterator< _Record >
	findByImpl(
			MakeIntegralConstant< _Member > _memberType
		, 	RemoveOptional< _FieldType > const & _value
		);

private:
	IDatabaseHandle m_database;

	ITableAccessorHandle m_tableHandle;
};

//---------------------------------------------------------------------------
template< class _Record >
DbTable< _Record >::DbTable( IDatabaseHandle const & _database )
	:	m_database( _database )
	,	m_tableHandle(
			_database->openTable(
					std::string( _Record::Bindings::TableName.begin(), _Record::Bindings::TableName.end() )
				,	_Record::Bindings::IsView
				)
			)
{
}

//---------------------------------------------------------------------------
template< class _Record >
int
DbTable< _Record >::size() const
{
	return m_tableHandle->size();
}

//---------------------------------------------------------------------------
template< class _Record >
std::optional< _Record >
DbTable< _Record >::find( std::int64_t _id ) const
{
	_Record result;

	if( !m_tableHandle->find( _id, *getRecordWraper( &result ) ) )
		return std::optional< _Record >();

	return result;
}

//---------------------------------------------------------------------------
template< class _Record >
DbRecordIterator< _Record >
DbTable< _Record >::getAll() const
{
	return DbRecordIterator< _Record >( m_tableHandle->getAll() );
}

//---------------------------------------------------------------------------
template< class _Record >
template<
		typename _FieldType
	,	_FieldType ( _Record::*_Member )
	>
DbRecordIterator< _Record >
DbTable< _Record >::findByImpl(
		MakeIntegralConstant< _Member > _memberType
	, 	RemoveOptional< _FieldType > const & _value
	)
{
	using FieldArgType = RemoveOptional< _FieldType >;

	static thread_local std::string fieldName;

	if( fieldName.empty() )
	{
		_Record record;
		_FieldType *field = &( record.*_Member );
		GetFieldName getFieldName( field, &fieldName );
		_Record::Bindings::visit( record, getFieldName );
	}

	auto getValueRef = []( FieldArgType const & _value2 )
	{
		if constexpr( std::is_pod_v< FieldArgType > )
			return _value2;
		else
			return &_value2;
	};

	return m_tableHandle->findBy( fieldName, &typeid( FieldArgType ), getValueRef( _value ) );
}

//---------------------------------------------------------------------------
template< class _Record >
void
DbTable< _Record >::remove( std::int16_t _id )
{
	m_tableHandle->remove( _id );
}

}

