/*
 * sql_string_iterator.cpp
 *
 *  Created on: 13 sty 2024
 *      Author: supcio
 */

#include "sql_string_iterator.hpp"


SqlStringIterator::SqlStringIterator( std::string const & str )
	: m_it( str.begin() )
	, m_end( str.end() )
	, m_str( str ) {}

int
SqlStringIterator::asIndex() const
{
	return m_index;
}

std::string::const_iterator
SqlStringIterator::asIterator() const
{
	return m_it;
}

bool
SqlStringIterator::atEnd() const
{
	return m_it == m_end;
}

int
SqlStringIterator::operator*() const
{
	return *m_it;
}


int
SqlStringIterator::safeCharacterAt( int _i ) const
{
	if( std::distance( m_it + _i, m_end ) < 1 )
		return 0;

	return *( m_it + _i );
}

std::string
SqlStringIterator::substr( int _len ) const
{
	return m_str.substr(  m_index, _len );
}

SqlStringIterator &
SqlStringIterator::operator++()
{
	++m_index;
	++m_it;
	return *this;
}

SqlStringIterator &
SqlStringIterator::operator+=( int _offset )
{
	m_index += _offset;
	m_it += _offset;
	return *this;
}

bool
SqlStringIterator::incrementUntil( std::string const & _pattern, NotFoundBehaviour _notFountBhaviour )
{
	size_t index = m_str.find( _pattern, m_index );

	if( index != std::string::npos )
	{
	  m_it += index - m_index;
	  m_index = index;
	  return true;
	}
	else if( _notFountBhaviour == NotFoundBehaviour::MoveToEnd )
	{
	  m_it = m_end;
	  m_index = m_str.size();
	  return true;
	}
	else
	  return false;
}

void
SqlStringIterator::moveToEnd()
{
	m_it = m_end;
	m_index = m_str.size();
}
