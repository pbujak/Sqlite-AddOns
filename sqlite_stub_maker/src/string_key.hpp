/*
 * string_key.hpp
 *
 *  Created on: 19 sty 2024
 *      Author: supcio
 */

#pragma once

#include "util.hpp"
#include <boost/algorithm/string/compare.hpp>
#include <type_traits>

struct CompareNoCase
{
	bool operator()( std::string const & _lhs, std::string const & _rhs ) const
	{
		return Util::compareNoCase( _lhs, _rhs ) < 0;
	}
};

template< bool _CaseSensitive >
using Less = std::conditional_t< _CaseSensitive, boost::is_less, boost::is_iless >;

template< auto _Key, bool _CaseSensitive = false >
struct CompareByField;

template< auto _GetKey, bool _CaseSensitive = false >
struct CompareByMethod;

//---------------------------------------------------------------------------------------
template< bool _CaseSensitive, typename _KeyType >
inline bool lessKey( _KeyType const & _lhs, _KeyType const & _rhs )
{
	if constexpr ( !std::is_convertible_v< _KeyType, std::string > )
		return _lhs < _rhs;
	else if constexpr ( _CaseSensitive )
		return _lhs < _rhs;
	else
		return Util::compareNoCase( _lhs, _rhs ) < 0;

}

//=======================================================================================
template<
        typename _Struct
    ,   typename _KeyType
    ,   _KeyType ( _Struct::*_Key )
    ,   bool _CaseSensitive
    >
struct CompareByField< _Key, _CaseSensitive >
{
    using is_transparent = void;

    template<
            typename _ArgKeyType
        ,   std::enable_if_t< std::is_convertible_v< _ArgKeyType, _KeyType >, int > _Enable = 1
    >
    inline static _KeyType getKey( _ArgKeyType _key )
    {
        return _key;
    }

    template<
            typename _Struct2
        ,   std::enable_if_t< std::is_convertible_v< _Struct2, _Struct >, int > _Enable = 1
    >
    inline static _KeyType getKey( _Struct2 const & _struct )
    {
        return ( _struct.*_Key );
    }

    template< typename _Lhs, typename _Rhs >
    bool operator()( _Lhs const & _lhs, _Rhs const & _rhs ) const
    {
        return lessKey< _CaseSensitive >( getKey( _lhs ), getKey( _rhs ) );
    }
};

//=======================================================================================
template<
        typename _Struct
	,   typename _KeyType
    ,   _KeyType ( _Struct::*_GetKey )() const
    ,   bool _CaseSensitive
    >
struct CompareByMethod< _GetKey, _CaseSensitive >
{
    using is_transparent = void;

    template<
            typename _ArgKeyType
        ,   std::enable_if_t< std::is_convertible_v< _ArgKeyType, _KeyType >, int > _Enable = 1
    >
    inline static _KeyType getKey( _ArgKeyType const & _string )
    {
        return _string;
    }

    template<
            typename _Struct2
        ,   std::enable_if_t< std::is_convertible_v< _Struct2, _Struct >, int > _Enable = 1
    >
    inline static _KeyType getKey( _Struct2 const & _struct )
    {
        return ( _struct.*_GetKey )();
    }

    template< typename _Lhs, typename _Rhs >
    bool operator()( _Lhs const & _lhs, _Rhs const & _rhs ) const
    {
        return lessKey< _CaseSensitive >( getKey( _lhs ), getKey( _rhs ) );
    }
};


