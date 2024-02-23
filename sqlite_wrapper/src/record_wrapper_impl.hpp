/*
 * record_wrapper_impl.hpp
 *
 *  Created on: 9 lut 2024
 *      Author: supcio
 */

#pragma once
#include "i_record_wrapper.hpp"
#include <string>
#include <vector>

namespace sqlw
{

namespace detail
{

//===============================================================================
class GetColumnsVisitor
{
public:
	inline GetColumnsVisitor( std::vector< std::string > & _columnNames, bool _onlyModifiable )
		:	m_columnNames( _columnNames )
		,	m_onlyModifiable( _onlyModifiable )
	{
	}

	template< typename _Value >
	inline void visit( std::string const & _columnName, _Value & _value, bool _modifiable )
	{
		if( !m_onlyModifiable || _modifiable )
			m_columnNames.push_back( _columnName );
	}

private:
	std::vector< std::string > & m_columnNames;

	bool m_onlyModifiable;
};

}

//===============================================================================
template< class _Record >
class RecordWrapperImpl : public IRecordWrapper
{
public:
	void setRecord( _Record * _record )
	{
		m_record = _record;
	}

	std::vector< std::string > getColumnNames( bool _onlyModifiable ) const override
	{
		std::vector< std::string > columnNames;

		detail::GetColumnsVisitor visitor( columnNames, _onlyModifiable );
		_Record::Bindings::visit( *m_record, visitor );
		return columnNames;
	}

	std::int64_t getId() const override
	{
		return m_record->m_id;
	}

	void setId( std::int64_t _id ) override
	{
		m_record->m_id = _id;
	}

	void accept( IRecordGettingVisitor & _visitor ) const override
	{
		_Record const & constRecord = *m_record;
		_Record::Bindings::visit( constRecord, _visitor );
	}

	void accept( IRecordSettingVisitor & _visitor ) override
	{
		_Record::Bindings::visit( *m_record, _visitor );
	}

private:
	_Record * m_record;
};

template< typename _Record >
IRecordWrapper* getRecordWraper( _Record * _record )
{
	static thread_local RecordWrapperImpl< _Record > staticWrapper;
	staticWrapper.setRecord( _record );
	return &staticWrapper;
}

}


