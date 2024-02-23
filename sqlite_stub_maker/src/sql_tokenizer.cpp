/*
 * sql_tokenizer.cpp
 *
 *  Created on: 13 sty 2024
 *      Author: supcio
 */

#include "sql_tokenizer.hpp"
#include <boost/algorithm/string.hpp>

namespace
{

//-------------------------------------------------------------------
SqlToken
makeResult( SqlTokenKind _kind, int _offset, std::string && _token )
{
	SqlToken result;
	result.m_kind = _kind;
	result.m_offset = _offset;
	result.m_content = std::move( _token );
	return result;
}

//-------------------------------------------------------------------
int
nextCharacter( SqlStringIterator & _iterator )
{
	if( _iterator.atEnd() )
		return 0;

	int ch = *_iterator;

	++_iterator;

	return ch;
}

}

//-------------------------------------------------------------------
SqlTokenizer::SqlTokenizer( std::string const & _sqlString )
	:	m_itString( _sqlString )
{
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::next()
{
	skipSpaces();

	int offset = m_itString.asIndex();

	if( m_itString.atEnd() )
		return std::optional< SqlToken > ();

	NextTokenMethod nextTokenMethods[]
		=	{
					&SqlTokenizer::nextWord
				,	&SqlTokenizer::nextLiteral
				,	&SqlTokenizer::nextHexadecimal
				,	&SqlTokenizer::nextNumeric
				,	&SqlTokenizer::nextVariable
				,	&SqlTokenizer::nextCommentBlock
				,	&SqlTokenizer::nextOperator
				,	&SqlTokenizer::nextSpecialCharacter
			};

	for( auto nextTokenMethod : nextTokenMethods )
	{
		auto result = ( this->*nextTokenMethod )();
		if( result )
			return result;
	}

	auto it = m_itString.asIterator();
	m_itString.moveToEnd();

	SqlToken unparsed;
	unparsed.m_kind = SqlTokenKind::Unparsed;
	unparsed.m_offset = offset;
	unparsed.m_content = std::string( it, m_itString.asIterator() );
	return unparsed;
}

//-------------------------------------------------------------------
void
SqlTokenizer::skipSpaces()
{
	m_itString.incrementWhile( []( char _ch ){ return std::isspace( _ch ); } );
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextWord()
{
	if( !std::isalpha( *m_itString ) && *m_itString != '_' )
		return std::optional< SqlToken >();

	auto isNextWordChar = []( char _ch )
	{
		return
				std::isalpha( _ch )
			|| 	std::isdigit( _ch )
			||	_ch == '_'
			||	_ch == '$'
			||	_ch == '@'
			||	_ch == '.';
	};

	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	m_itString.incrementWhile( isNextWordChar );

	return makeResult(
				SqlTokenKind::Word
			, 	offset
			, 	std::string( it, m_itString.asIterator() )
		);
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextHexadecimal()
{
	if( boost::algorithm::to_lower_copy( m_itString.substr( 2 ) ) != "0x" )
		return std::optional< SqlToken >();

	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	m_itString += 2;

	m_itString.incrementWhile(
		[]( char _ch )
		{
			if( std::isdigit( _ch ) )
				return true;

			static std::string hexLetters = "abcdefABCDEF";

			return hexLetters.find( _ch ) != std::string::npos;
		}
	);

	return makeResult(
				SqlTokenKind::Numeric
			, 	offset
			, 	std::string( it, m_itString.asIterator() )
		);
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextNumeric()
{
	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	auto digit = []( char _ch ) -> bool
	{
		if( _ch == '.' )
			return true;

		return std::isdigit( _ch );
	};

	if( std::isdigit( *m_itString ) )
	{
		m_itString.incrementWhile( digit );

		m_itString.incrementWhile(
			[]( char _ch )
			{
				return std::tolower( _ch == 'e' );
			}
		);

		m_itString.incrementWhile(
			[]( char _ch )
			{
				return _ch == '+' || _ch == '-';
			}
		);

		m_itString.incrementWhile( digit );

		return makeResult(
					SqlTokenKind::Numeric
				, 	offset
				, 	std::string( it, m_itString.asIterator() )
			);
	}

	if(
			*m_itString == '.'
		&& 	std::isdigit( m_itString.safeCharacterAt( 1 ) )
		)
	{
		++m_itString;
		m_itString.incrementWhile( digit );

		return makeResult(
					SqlTokenKind::Numeric
				, 	offset
				, 	std::string( it, m_itString.asIterator() )
			);
	}

	return std::optional< SqlToken >();
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextLiteral()
{
	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	auto isQuoteChar = []( char _ch )
	{
		return _ch == '"' || _ch == '\'' || _ch == '`';
	};

	char firstChar = *m_itString;

	if( !isQuoteChar( firstChar ) )
		return std::optional< SqlToken > ();

	++m_itString;

	while( true )
	{
		int ch = nextCharacter( m_itString );

		if( ch == '\\' )
			nextCharacter( m_itString );

		if( ch == firstChar )
			break;

		if( ch == 0 )
		{
			return makeResult(
						SqlTokenKind::Unparsed
					, 	offset
					, 	std::string( it, m_itString.asIterator() )
				);
		}
	}

	return makeResult(
				SqlTokenKind::Literal
			, 	offset
			, 	std::string( it, m_itString.asIterator() )
		);
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextVariable()
{
	if( *m_itString != '@' || !std::isalpha( m_itString.safeCharacterAt( 1 ) ) )
		return std::optional< SqlToken >();

	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	m_itString += 2;

	auto isVariableChar = []( char _ch )
	{
		if( std::isalpha( _ch ) || std::isdigit( _ch ) )
			return true;

		return _ch == '_' || _ch == '$';
	};

	m_itString.incrementWhile( isVariableChar );

	return makeResult(
				SqlTokenKind::Variable
			, 	offset
			, 	std::string( it, m_itString.asIterator() )
		);
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextOperator()
{
	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	auto isOperator = []( char _ch )
	{
		static std::string operators = { "+-*/<>=!%~&|" };

		return operators.find( _ch ) != std::string::npos;
	};

	if( !isOperator( *m_itString ) )
		return std::optional< SqlToken >();

	m_itString.incrementWhile( isOperator );

	return makeResult(
				SqlTokenKind::Operator
			, 	offset
			, 	std::string( it, m_itString.asIterator() )
		);
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextSpecialCharacter()
{
	static std::unordered_map< int, SqlTokenKind > mapCharToToken
		=	{
					{ ',', SqlTokenKind::Comma }
				,	{ ';', SqlTokenKind::SemiColon }
				,	{ '(', SqlTokenKind::Bracket }
				,	{ ')', SqlTokenKind::Bracket }
				,	{ '[', SqlTokenKind::Bracket }
				,	{ ']', SqlTokenKind::Bracket }
			};

	auto it = mapCharToToken.find( *m_itString );
	if( it == mapCharToToken.end() )
		return std::optional< SqlToken >();

	SqlToken result;
	result.m_kind = it->second;
	result.m_offset = m_itString.asIndex();
	result.m_content = *m_itString;
	++m_itString;

	return result;
}

//-------------------------------------------------------------------
std::optional< SqlToken >
SqlTokenizer::nextCommentBlock()
{
	int offset = m_itString.asIndex();
	auto it = m_itString.asIterator();

	if( m_itString.substr( 2 ) == "--" )
	{
		auto isNewLine = []( char _ch )
		{
			return _ch == '\n' || _ch == '\r';
		};

		m_itString.incrementUntil( isNewLine, NotFoundBehaviour::MoveToEnd );

		return makeResult(
					SqlTokenKind::CommentBlock
				, 	offset
				, 	std::string( it, m_itString.asIterator() )
			);
	}

	if( m_itString.substr( 2 ) == "/*" )
	{
		if( !m_itString.incrementUntil( "*/", NotFoundBehaviour::Ignore ) )
			return std::optional< SqlToken >();

		m_itString += 2;

		return makeResult(
					SqlTokenKind::CommentBlock
				, 	offset
				, 	std::string( it, m_itString.asIterator() )
			);
	}
	return std::optional< SqlToken >();
}

