/*
 * db_table_auxiliary.hpp
 *
 *  Created on: 12 lut 2024
 *      Author: supcio
 */

#pragma once
#include <string>
#include <optional>
#include <type_traits>

namespace sqlw
{

//=================================================================
template< typename _FieldType >
class GetFieldName
{
public:
	GetFieldName( _FieldType * _field, std::string * _fieldName )
		:	m_field( _field )
		,	m_fieldName( _fieldName )
	{
	}

	template< typename _FieldType2 >
	void visit( std::string const & _name, _FieldType2 & _field, bool _modified )
	{
		if constexpr( std::is_convertible_v< _FieldType2&, _FieldType& > )
		{
			if( &_field == m_field )
				*m_fieldName = _name;
		}
	}

private:
	_FieldType * m_field;

	std::string * m_fieldName;
};

template< auto _Value >
using MakeIntegralConstant = std::integral_constant< decltype( _Value ), _Value >;

template < typename _Value >
struct RemoveOptionalImpl
{
	using type = _Value;
};

template < typename _Value >
struct RemoveOptionalImpl< std::optional< _Value > >
{
	using type = _Value;
};

template < typename _Value >
using RemoveOptional = typename RemoveOptionalImpl< _Value >::type;

}
