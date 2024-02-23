#pragma once
#include <string>
#include <algorithm>

//======================================================================
enum class NotFoundBehaviour
{
    Ignore
  , MoveToEnd
};

//======================================================================
class SqlStringIterator
{
public:
  SqlStringIterator( std::string const & str );

  int asIndex() const;

  std::string::const_iterator asIterator() const;

  bool atEnd() const;

  int operator*() const;

  int safeCharacterAt( int _i ) const;

  std::string substr( int _len ) const;

  SqlStringIterator & operator++();

  SqlStringIterator & operator+=( int _offset );

  template< typename _Condition >
  void incrementWhile( _Condition const & _condition );

  template<
      typename _Condition
    , typename _Enable = decltype( std::declval< _Condition >()( ( char )0 ) )
  >
  bool incrementUntil( _Condition const & _condition, NotFoundBehaviour _notFountBehaviour );

  bool incrementUntil( std::string const & _pattern, NotFoundBehaviour _notFountBehaviour );

  void moveToEnd();

private:
  int m_index = 0;
  std::string::const_iterator m_it;
  std::string::const_iterator m_end;
  std::string const& m_str;
};

//------------------------------------------------------------------------
template< typename _Condition >
void
SqlStringIterator::incrementWhile( _Condition const & _condition )
{
	while( m_it != m_end )
	{
		if( !_condition( *m_it ) )
			return;

		++m_it;
		++m_index;
	}
}

//------------------------------------------------------------------------
template<
		typename _Condition
	, 	typename _Enable
>
bool
SqlStringIterator::incrementUntil( _Condition const & _condition, NotFoundBehaviour _notFountBhaviour )
{
	auto it = std::find_if( m_it, m_end, _condition );

	if( it != m_end || _notFountBhaviour == NotFoundBehaviour::MoveToEnd  )
	{
	  m_index += std::distance( m_it, it );
	  m_it = it;
	  return true;
	}
	else
	  return false;
}
