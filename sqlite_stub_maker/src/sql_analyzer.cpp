/*
 * sql_analyzer.cpp
 *
 *  Created on: 16 sty 2024
 *      Author: supcio
 */
#include "sql_analyzer.hpp"
#include "util.hpp"

//---------------------------------------------------------------------
SqlAnalyzer::SqlAnalyzer( std::vector< SqlToken > const & _tokens )
	:	m_tokens( _tokens )
	,	m_it( _tokens.begin() )
	,	m_end( _tokens.end() )
{
}

//---------------------------------------------------------------------
bool
SqlAnalyzer::nextTokensLike( std::vector< std::string > const & _expectedWords, bool _moveAfterIfOk )
{
	int count = _expectedWords.size();

	auto it2 = m_it;

	int accepted = 0;

	for( int i = 0; i < count && it2 < m_end; ++i, ++it2 )
	{
		if( !Util::equalsNoCase( it2->m_content, _expectedWords[ i ] ) )
			return false;

		++accepted;
	}

	if( _moveAfterIfOk )
		m_it += accepted;

	return true;
}

//---------------------------------------------------------------------
bool
SqlAnalyzer::nextTokensLike( SqlTokenKind _tokenKind, bool _moveAfterIfOk )
{
	if( m_it == m_end )
		return false;

	if( m_it->m_kind != _tokenKind )
		return false;

	if( _moveAfterIfOk )
		++m_it;

	return true;
}

//---------------------------------------------------------------------
std::optional< std::string >
SqlAnalyzer::nextIdentifier()
{
	if( m_it == m_end )
		return std::optional< std::string >();

	if( m_it->m_kind != SqlTokenKind::Word )
		return std::optional< std::string >();

	return ( m_it++ )->m_content;
}

//---------------------------------------------------------------------
std::optional< std::string >
SqlAnalyzer::nextVariable()
{
	if( m_it == m_end )
		return std::optional< std::string >();

	if( m_it->m_kind != SqlTokenKind::Variable )
		return std::optional< std::string >();

	return ( m_it++ )->m_content;
}

//---------------------------------------------------------------------
bool
SqlAnalyzer::findAndJumpAfter( std::vector< std::string > const & _expectedWords )
{
	auto it = m_it;
	auto it2 = it;

	while( it2 != m_end )
	{
		if( Util::equalsNoCase( it2->m_content, _expectedWords[ 0 ] ) )
		{
			m_it = it2;
			if( nextTokensLike( _expectedWords ) )
				return true;
			m_it = it;
		}
		++it2;
	}

	return false;
}

//---------------------------------------------------------------------
bool
SqlAnalyzer::findAndJumpAfter( FindCondition const & _findCondition )
{
	auto it2 = m_it;

	while( it2 != m_end )
	{
		if( _findCondition( it2, m_end ) )
		{
			m_it = it2;
			return true;
		}
		++it2;
	}

	return false;
}

//---------------------------------------------------------------------
std::vector< SqlToken > const * const
SqlAnalyzer::getUnderlyingTokens() const
{
	return &m_tokens;
}

//---------------------------------------------------------------------
std::vector< SqlToken >::const_iterator
SqlAnalyzer::current() const
{
	return m_it;
}

//---------------------------------------------------------------------
bool
SqlAnalyzer::atEnd() const
{
	return m_it == m_end;
}
