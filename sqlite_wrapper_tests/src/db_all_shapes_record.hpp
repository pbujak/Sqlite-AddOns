#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>
#include <string>

namespace dbSymbols
{

// ORM mappings for SQLite table/view "ALL_shapes"	
struct DbAllShapesRecord
{
	struct Bindings;

	// id
	std::int64_t m_id = -1;
	
	// unit
	std::string m_unit;

	// symbol
	std::string m_symbol;

	// guid
	std::string m_guid;

	// content
	std::string m_content;

};
	
struct DbAllShapesRecord::Bindings
{
	static std::string_view constexpr TableName = "ALL_shapes";
	
	static bool constexpr IsView = true;
	static bool constexpr IsModifiable = true;
	
	template<
			class _Record
		,	class _Visitor
		,	std::enable_if_t< std::is_convertible_v< _Record &, DbAllShapesRecord const & >, bool > _Enable = true
		>
	static void visit( _Record & _record, _Visitor & _visitor )
	{
		_visitor.visit( "unit", _record.m_unit, false );
		_visitor.visit( "symbol", _record.m_symbol, false );
		_visitor.visit( "guid", _record.m_guid, true );
		_visitor.visit( "content", _record.m_content, true );
	}
};
	
bool
operator==( DbAllShapesRecord const & _lhs, DbAllShapesRecord const & _rhs )
{
	if( _lhs.m_id != _rhs.m_id )
		return false;
	if( _lhs.m_unit != _rhs.m_unit )
		return false;
	if( _lhs.m_symbol != _rhs.m_symbol )
		return false;
	if( _lhs.m_guid != _rhs.m_guid )
		return false;
	if( _lhs.m_content != _rhs.m_content )
		return false;
	return true;
}

}

