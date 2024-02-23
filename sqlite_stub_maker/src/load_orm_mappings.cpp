/*
 * load_orm_mappings.cpp
 *
 *  Created on: 11 sty 2024
 *      Author: supcio
 */
#include "load_orm_mappings.hpp"
#include "error_handling.hpp"
#include "xml_node_analyzer_impl.hpp"
#include "util.hpp"
#include <boost/algorithm/string.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <memory>

namespace
{


//-----------------------------------------------------------------------------------------------------------------------------------------
std::string toString( xmlChar const* const _xmlChar )
{
	return std::string( ( char const* const )_xmlChar );
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void
analyzeNodeTree( xmlNodePtr _node, std::shared_ptr< IXmlNodeAnalyzer > const & _analyzer )
{
	std::string nodeName = boost::algorithm::to_lower_copy( toString( _node->name ) );

	AttributeMap attributes;

	auto attrNode = _node->properties;

	while( attrNode )
	{
		std::string name = boost::algorithm::to_lower_copy( toString( attrNode->name ) );
		std::string value = toString( xmlGetProp( _node, attrNode->name ) );

		attributes.emplace( std::move( name ), std::move( value ) );

		attrNode = attrNode->next;
	}

	auto childAnalyzer = _analyzer->enterChildNode( _node->line, nodeName, std::move( attributes ) );

	if( !childAnalyzer )
	{
		if( _node->children )
		{
			std::stringstream ss;

			ss << "XML node " << nodeName << " has unexpected children.";

			ErrorHandling::throwError( ErrorInfo( "XML", ss.str() ).setLocation( _node->line ) );
		}
	}

	auto child = _node->children;
	if( !child )
		return;

	while( true )
	{
		if( child->type == XML_ELEMENT_NODE )
			analyzeNodeTree( child, childAnalyzer );

		if( child == _node->last )
			return;

		child = child->next;

		if( !child )
			return;
	}
}

}

//----------------------------------------------------------------------------------------------
OrmMappingsData
loadOrmMappings( std::filesystem::path const & _path )
{
	ScopedContextFile contextFile( _path.string() );

	std::shared_ptr< _xmlParserCtxt > parser( xmlNewParserCtxt(), xmlFreeParserCtxt );

	std::shared_ptr< _xmlDoc > xml(
			xmlCtxtReadFile(
					parser.get()
				,	_path.string().c_str()
				,	nullptr
				,	XML_PARSE_PEDANTIC | XML_PARSE_NOERROR | XML_PARSE_NOWARNING
			)
		,
			xmlFreeDoc
	);

	if( !xml )
	{
		auto lastError = &parser->lastError;

		ErrorHandling::throwError(
			ErrorInfo( "XML", lastError->message )
				.setLocation( lastError->line )
				.setCode( lastError->code )
		);
	}

	OrmMappingsData result;

	auto rootAnalyzer = getRootNodeAnalyzer( &result );

	analyzeNodeTree(  xmlDocGetRootElement( xml.get() ), rootAnalyzer );

	for( auto const & table : result.m_tables )
	{
		if( table.m_xmlRawFields.empty() )
		{
			std::stringstream ss;

			ss << "Table " << Util::quote( table.m_name ) << " has no declared mappings for columns.";

			ErrorHandling::reportWarning(
					ErrorInfo( "ORM", ss.str() )
						.setLocation( table.m_xmlOrmLine )
				);
		}
	}

	result.m_sourceXmlFile = _path.string();

	return result;
}



