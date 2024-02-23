/*
 * column_data_provider.cpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#include "private/column_data_provider.hpp"

namespace sqlw
{

namespace
{

//---------------------------------------------------------------------------------------------
template< typename _Value >
_Value
loadFromStatement( sqlite3_stmt* const _stmt, int _columnIndex )
{
	if constexpr( std::is_convertible_v< _Value, std::int64_t > )
		return sqlite3_column_int64( _stmt, _columnIndex );
	else if constexpr( std::is_convertible_v< _Value, double > )
		return sqlite3_column_double( _stmt, _columnIndex );
	else if constexpr( std::is_convertible_v< _Value, std::string > )
	{
		char const * text = reinterpret_cast< char const* >( sqlite3_column_text( _stmt, _columnIndex ) );
		return std::string( text ? text : "" );
	}
}

//---------------------------------------------------------------------------------------------
template< typename _Value >
std::optional< _Value >
loadOptionalFromStatement( sqlite3_stmt* const _stmt, int _columnIndex )
{
	if( sqlite3_column_type( _stmt, _columnIndex ) == SQLITE_NULL )
		return std::optional< _Value >();

	return loadFromStatement< _Value >( _stmt, _columnIndex );
}

//=============================================================================================
template< typename _GetColumnId >
class FillingVisitor : public IRecordSettingVisitor
{
public:
	FillingVisitor( sqlite3_stmt* const _stmt, _GetColumnId const & _getColumnId )
		:	m_stmt( _stmt)
		,	m_getColumnId( _getColumnId )
	{
	};

	void visit( std::string const & _fieldName, std::int64_t & _value, bool _modifiable ) override
	{
		_value = loadFromStatement< std::int64_t >( m_stmt, m_getColumnId( _fieldName ) );
	}

	void visit( std::string const & _fieldName, double & _value, bool _modifiable ) override
	{
		_value = loadFromStatement< double >( m_stmt, m_getColumnId( _fieldName ) );
	}

	void visit( std::string const & _fieldName, std::string & _value, bool _modifiable ) override
	{
		_value = loadFromStatement< std::string >( m_stmt, m_getColumnId( _fieldName ) );
	}

	void visit( std::string const & _fieldName, std::optional< std::int64_t > & _value, bool _modifiable ) override
	{
		_value = loadOptionalFromStatement< std::int64_t >( m_stmt, m_getColumnId( _fieldName ) );
	}

	void visit( std::string const & _fieldName, std::optional< double > & _value, bool _modifiable ) override
	{
		_value = loadOptionalFromStatement< double >( m_stmt, m_getColumnId( _fieldName ) );
	}

	void visit( std::string const & _fieldName, std::optional< std::string > & _value, bool _modifiable ) override
	{
		_value = loadOptionalFromStatement< std::string >( m_stmt, m_getColumnId( _fieldName ) );
	}

private:
	sqlite3_stmt* const m_stmt;

	_GetColumnId const & m_getColumnId;
};

}

//---------------------------------------------------------------------------------------------
ColumnDataProvider::ColumnDataProvider()
{
	// TODO Auto-generated constructor stub

}

//---------------------------------------------------------------------------------------------
ColumnDataProvider::~ColumnDataProvider()
{
	// TODO Auto-generated destructor stub
}

//---------------------------------------------------------------------------------------------
void
ColumnDataProvider::load( sqlite3_stmt* const _stmt, IRecordWrapper& _record )
{
	auto callGetColumnId = [ this, _stmt ]( std::string const _name )
		{
			return getColumnId( _stmt, _name );
		};

	std::int64_t recordId = sqlite3_column_int64( _stmt, getColumnId( _stmt, "id" ) );

	_record.setId( recordId );


	FillingVisitor< decltype( callGetColumnId ) > fillingVisitor( _stmt, callGetColumnId );

	_record.accept( fillingVisitor );
}

//---------------------------------------------------------------------------------------------
int
ColumnDataProvider::getColumnId( sqlite3_stmt* const a_stmt, std::string const & _pszColName )
{
	if( m_mapColumns.size() == 0 )
	{
		int count = sqlite3_column_count( a_stmt );

		for( int ctr = 0; ctr < count; ctr++ )
		{
			const char *szName = sqlite3_column_name( a_stmt, ctr );
			m_mapColumns[ szName ] = ctr;
		}
	}
	return m_mapColumns[ _pszColName ];
}

} /* namespace sqlw */
