/*
 * xml_analyzer.cpp
 *
 *  Created on: 12 sty 2024
 *      Author: supcio
 */

#include "xml_node_analyzer.hpp"
#include "error_handling.hpp"
#include "util.hpp"
#include <sstream>

//----------------------------------------------------------------------------------------------
XmlNodeAnalyzerContext::XmlNodeAnalyzerContext(
		int _line
	,	std::string const & _node
	,	AttributeMap && _attributes
)
	:	m_line( _line )
	,	m_node( _node )
	,	m_attributes( std::move( _attributes ) )
{
}

//----------------------------------------------------------------------------------------------
void
XmlNodeAnalyzerContext::checkNodeName( std::string const & _expectedName ) const
{
	if( m_node != _expectedName )
	{
		std::stringstream ss;
		ss << "Unexpected node name " << Util::quote( m_node ) << ", can be only " << Util::quote( _expectedName ) << ".";

		throwXmlError( ss.str() );
	}
}

//----------------------------------------------------------------------------------------------
std::string
XmlNodeAnalyzerContext::popAttribute( std::string const & _name )
{
	auto it = m_attributes.find( _name );

	if( it == m_attributes.end() )
	{
		std::stringstream ss;
		ss << "Expected attribute " << Util::quote( _name ) << " for node " << Util::quote( m_node ) << " not found.";

		throwXmlError( ss.str() );
	}

	std::string result = std::move( it->second );

	m_attributes.erase( it );

	return result;
}

//----------------------------------------------------------------------------------------------
std::optional< std::string >
XmlNodeAnalyzerContext::popAttributeOptional( std::string const & _name )
{
	auto it = m_attributes.find( _name );

	if( it == m_attributes.end() )
		return std::optional< std::string >();

	std::string result = std::move( it->second );

	m_attributes.erase( it );

	return result;
}

//----------------------------------------------------------------------------------------------
void
XmlNodeAnalyzerContext::checkNoMoreAttributes() const
{
	if( !m_attributes.empty() )
	{
		std::stringstream ss;
		ss << "Node " << Util::quote( m_node ) << " has unexpected attributes.";

		throwXmlError( ss.str() );
	}

}

//----------------------------------------------------------------------------------------------
void
XmlNodeAnalyzerContext::throwXmlError( std::string const & _message ) const
{
	ErrorHandling::throwError( ErrorInfo( "XML", _message ).setLocation( m_line ) );
};


