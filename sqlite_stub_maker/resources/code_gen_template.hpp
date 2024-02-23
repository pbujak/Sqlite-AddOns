#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>
[%FOR-EACH%](include)
#include [$Include$]
[%END%]

namespace [$Namespace$]
{

// ORM mappings for SQLite table/view "[$TableName$]"	
struct [$RecordClass$]
{
	struct Bindings;

	// id
	std::int64_t m_id = -1;
	
	[%FOR-EACH%](field)
	// [$ColumnName$]
	[$cppFieldType$] [$cppFieldName$][$cppFieldInit$];

	[%END%]
};
	
struct [$RecordClass$]::Bindings
{
	static std::string_view constexpr TableName = "[$TableName$]";
	
	static bool constexpr IsView = [$IsView$];
	static bool constexpr IsModifiable = [$TableModifiable$];
	
	template<
			class _Record
		,	class _Visitor
		,	std::enable_if_t< std::is_convertible_v< _Record &, [$RecordClass$] const & >, bool > _Enable = true
		>
	static void visit( _Record & _record, _Visitor & _visitor )
	{
		[%FOR-EACH%](field)
		_visitor.visit( "[$ColumnName$]", _record.[$cppFieldName$], [$ColumnModifiable$] );
		[%END%]
	}
};
	
inline bool
operator==( [$RecordClass$] const & _lhs, [$RecordClass$] const & _rhs )
{
	if( _lhs.m_id != _rhs.m_id )
		return false;
	[%FOR-EACH%](field)
	if( _lhs.[$cppFieldName$] != _rhs.[$cppFieldName$] )
		return false;
	[%END%]
	return true;
}

}

