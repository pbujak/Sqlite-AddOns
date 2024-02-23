#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>
#include <string>

namespace dbSymbols
{

// ORM mappings for SQLite table/view "units"	
struct DbUnitRecord
{
	struct Bindings;

	// id
	std::int64_t m_id = -1;
	
	// name
	std::string m_name;

};
	
struct DbUnitRecord::Bindings
{
	static std::string_view constexpr TableName = "units";
	
	static bool constexpr IsView = false;
	static bool constexpr IsModifiable = true;
	
	template<
			class _Record
		,	class _Visitor
		,	std::enable_if_t< std::is_convertible_v< _Record &, DbUnitRecord const & >, bool > _Enable = true
		>
	static void visit( _Record & _record, _Visitor & _visitor )
	{
		_visitor.visit( "name", _record.m_name, true );
	}
};
	
inline bool
operator==( DbUnitRecord const & _lhs, DbUnitRecord const & _rhs )
{
	if( _lhs.m_id != _rhs.m_id )
		return false;
	if( _lhs.m_name != _rhs.m_name )
		return false;
	return true;
}

}

