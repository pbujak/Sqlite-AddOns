/*
 * get_pointer.hpp
 *
 *  Created on: 28 sty 2024
 *      Author: supcio
 */

#pragma once
#include <memory>
#include <type_traits>
#include <sqlite3.h>

namespace detail
{

//-----------------------------------------------------------
template< typename _SqliteObject >
void
deleteSqliteObject( _SqliteObject * _object )
{
	if constexpr ( std::is_convertible_v< _SqliteObject, sqlite3 > )
		sqlite3_close( _object );
	else if constexpr ( std::is_convertible_v< _SqliteObject, sqlite3_stmt > )
		sqlite3_finalize( _object );
	else if constexpr ( std::is_convertible_v< _SqliteObject, sqlite3_backup > )
		sqlite3_backup_finish( _object );
}

}

//===========================================================
template< typename _SqliteObject >
class SqliteGetHandle;

//==============================================================================
template< typename _SqliteObject >
class SqliteHandle
{
public:
	inline SqliteHandle() = default;

	inline SqliteHandle( _SqliteObject * const _object )
		:	m_object( _object, detail::deleteSqliteObject< _SqliteObject > )
	{
	}

	inline void reset()
	{
		m_object.reset();
	}

	inline void reset( _SqliteObject * const _object )
	{
		m_object.reset( _object, detail::deleteSqliteObject< _SqliteObject > );
	}

	inline operator _SqliteObject * const () const
	{
		return m_object.get();
	}

	inline SqliteGetHandle< _SqliteObject > operator & ()
	{
		return SqliteGetHandle( *this );
	}

	inline int getUseCount() const
	{
		return m_object.use_count();
	}

private:
	std::shared_ptr< _SqliteObject > m_object;
};

//===========================================================
template< typename _SqliteObject >
class SqliteGetHandle
{
public:
	SqliteGetHandle( SqliteHandle< _SqliteObject > & _object )
		:	m_object( _object )
	{
	}

	~SqliteGetHandle()
	{
		if( m_newValue )
			m_object.reset( m_newValue );
	}

	operator _SqliteObject** ()
	{
		return &m_newValue;
	}

private:
	SqliteHandle< _SqliteObject > & m_object;

	_SqliteObject * m_newValue = nullptr;
};


