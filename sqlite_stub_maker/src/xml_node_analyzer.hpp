/*
 * xml_analyzer.hpp
 *
 *  Created on: 11 sty 2024
 *      Author: supcio
 */

#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

using AttributeMap = std::unordered_map< std::string, std::string >;

//==============================================================================================
struct IXmlNodeAnalyzer
{
	virtual ~IXmlNodeAnalyzer() = default;

	virtual std::shared_ptr< IXmlNodeAnalyzer > enterChildNode(
			int _line
		,	std::string const & _node
		,	AttributeMap && _attributes
		) = 0;
};

//==============================================================================================
class XmlNodeAnalyzerContext
{
public:
	XmlNodeAnalyzerContext(
			int _line
		,	std::string const & _node
		,	AttributeMap && _attributes
		);

	void checkNodeName( std::string const & _expectedName ) const;

	std::string popAttribute( std::string const & _name );

	std::optional< std::string > popAttributeOptional( std::string const & _name );

	void checkNoMoreAttributes() const;

private:
	[[ noreturn ]] void throwXmlError( std::string const & _message ) const;

private:
	int m_line;

	std::string m_node;

	AttributeMap m_attributes;
};
