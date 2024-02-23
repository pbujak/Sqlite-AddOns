/*
 * xml_node_analyzer_impl.cpp
 *
 *  Created on: 12 sty 2024
 *      Author: supcio
 */

#include "xml_node_analyzer_impl.hpp"
#include "error_handling.hpp"
#include "string_key.hpp"
#include "util.hpp"

#include <boost/algorithm/string.hpp>
#include <unordered_set>
#include <map>

namespace
{

//----------------------------------------------------------------------------------------------
bool
getBoolean( int _line, std::string const & _asString )
{
	static std::map< std::string, bool, CompareNoCase > mapStringToBool
		=	{
					{ "1", true }
				,	{ "y", true }
				,	{ "yes", true }
				,	{ "true", true }
				,	{ "t", true }
				,	{ "0", false }
				,	{ "n", false }
				,	{ "no", false }
				,	{ "false", false }
				,	{ "f", false }
			};

	auto it = mapStringToBool.find( _asString );
	if( it == mapStringToBool.end() )
	{
		ErrorHandling::throwError(
				ErrorInfo( "ORM", Util::quote( _asString ) + " is not corrent boolean value." )
					.setLocation( _line )
			);
	}
	return it->second;
}

//----------------------------------------------------------------------------------------------
template< typename _FormatMessage >
void
addUniqueOrThrowError(
		std::unordered_set< std::string > & _set
	,	std::string const & _key
	,	int _line
	,	_FormatMessage const & _formatMessage
	)
{
	if( _set.find( _key ) != _set.end() )
	{
		std::string message = _formatMessage( _key );

		ErrorHandling::throwError(
				ErrorInfo( "ORM", message )
					.setLocation( _line )
			);
	}

	_set.insert( _key );
}

//==============================================================================================
class XmlColumnCollector : public IXmlNodeAnalyzer
{
public:
	XmlColumnCollector( TableData * const _tableData );

	std::shared_ptr< IXmlNodeAnalyzer > enterChildNode(
			int _line
		,	std::string const & _node
		,	AttributeMap && _attributes
		) override;

private:
	TableData * const m_tableData;

	std::unordered_set< std::string > m_uniqueColumns;
	std::unordered_set< std::string > m_uniqueFields;
};

//----------------------------------------------------------------------------------------------
XmlColumnCollector::XmlColumnCollector( TableData * const _tableData )
	:	m_tableData( _tableData )
{
}

//----------------------------------------------------------------------------------------------
std::shared_ptr< IXmlNodeAnalyzer >
XmlColumnCollector::enterChildNode(
		int _line
	,	std::string const & _node
	,	AttributeMap && _attributes
	)
{
	XmlNodeAnalyzerContext context( _line, _node, std::move( _attributes ) );

	context.checkNodeName( "column" );
	auto name = context.popAttribute( "name" );
	auto field = context.popAttribute( "field" );
	auto notnull = context.popAttributeOptional( "notnull" );
	context.checkNoMoreAttributes();

	auto formatDuplicateColumnMessage = []( std::string const & _column )
	{
		return std::string( "Duplicate column name " ) + Util::quote( _column );
	};

	auto formatDuplicateRecordMessage = []( std::string const & _field )
	{
		return std::string( "Duplicate member class name " ) + Util::quote( _field );
	};

	addUniqueOrThrowError( m_uniqueColumns, boost::to_upper_copy( name ), _line,  formatDuplicateColumnMessage );
	addUniqueOrThrowError( m_uniqueFields, field, _line,  formatDuplicateRecordMessage );

	if( Util::equalsNoCase( name, "ID" ) )
	{
		ErrorHandling::throwError(
				ErrorInfo( "ORM", "Column 'ID' is handled in special manner." )
					.setLocation( _line )
			);
	}

	FieldData fieldData;
	fieldData.m_xmlOrmLine = _line;
	fieldData.m_name = std::move( name );
	fieldData.m_cppMember = std::move( field );

	if( notnull )
		fieldData.m_notNull = getBoolean( _line, *notnull );

	m_tableData->m_xmlRawFields.insert( std::move( fieldData ) );

	return nullptr;
}

//==============================================================================================
class XmlTableCollector : public IXmlNodeAnalyzer
{
public:
	XmlTableCollector( OrmMappingsData * const _mappingsData );

	std::shared_ptr< IXmlNodeAnalyzer > enterChildNode(
			int _line
		,	std::string const & _node
		,	AttributeMap && _attributes
		) override;

private:
	OrmMappingsData * const m_mappingsData;

	std::unordered_set< std::string > m_uniqueTables;
	std::unordered_set< std::string > m_uniqueClasses;
};

//----------------------------------------------------------------------------------------------
XmlTableCollector::XmlTableCollector( OrmMappingsData * const _mappingsData )
	:	m_mappingsData( _mappingsData )
{
}

//----------------------------------------------------------------------------------------------
std::shared_ptr< IXmlNodeAnalyzer >
XmlTableCollector::enterChildNode(
		int _line
	,	std::string const & _node
	,	AttributeMap && _attributes
	)
{
	XmlNodeAnalyzerContext context( _line, _node, std::move( _attributes ) );

	context.checkNodeName( "table" );
	auto name = context.popAttribute( "name" );
	auto cls = context.popAttribute( "class" );
	context.checkNoMoreAttributes();

	auto formatDuplicateTableMessage = []( std::string const & _table )
	{
		return std::string( "Duplicate table name " ) + Util::quote( _table );
	};

	auto formatDuplicateRecordMessage = []( std::string const & _record )
	{
		return std::string( "Duplicate record class name " ) + Util::quote( _record );
	};

	addUniqueOrThrowError( m_uniqueTables, boost::to_upper_copy( name ), _line,  formatDuplicateTableMessage );
	addUniqueOrThrowError( m_uniqueClasses, cls, _line,  formatDuplicateRecordMessage );

	TableData tableData;
	tableData.m_xmlOrmLine = _line;
	tableData.m_name = std::move( name );
	tableData.m_cppClass = std::move( cls );

	m_mappingsData->m_tables.push_back( std::move( tableData ) );

	return std::make_shared< XmlColumnCollector >( &( *m_mappingsData->m_tables.rbegin() ) );
}


//==============================================================================================
class XmlNodeRootAnalyzer : public IXmlNodeAnalyzer
{
public:
	XmlNodeRootAnalyzer( OrmMappingsData * const _mappingsData );

	std::shared_ptr< IXmlNodeAnalyzer > enterChildNode(
			int _line
		,	std::string const & _node
		,	AttributeMap && _attributes
		) override;

private:
	OrmMappingsData * const m_mappingsData;
};

//----------------------------------------------------------------------------------------------
XmlNodeRootAnalyzer::XmlNodeRootAnalyzer( OrmMappingsData * const _mappingsData )
	:	m_mappingsData( _mappingsData )
{
}

//----------------------------------------------------------------------------------------------
std::shared_ptr< IXmlNodeAnalyzer >
XmlNodeRootAnalyzer::enterChildNode(
		int _line
	,	std::string const & _node
	,	AttributeMap && _attributes
	)
{
	XmlNodeAnalyzerContext context( _line, _node, std::move( _attributes ) );

	context.checkNodeName( "orm-mappings" );
	auto ns = context.popAttributeOptional( "namespace" );
	context.checkNoMoreAttributes();

	m_mappingsData->m_namespace = ns ? *ns : "Database";

	return std::make_shared< XmlTableCollector >( m_mappingsData );
}

}

//-------------------------------------------------------------------------------------------------------------------
std::shared_ptr< IXmlNodeAnalyzer >
getRootNodeAnalyzer( OrmMappingsData * const _mappingsData )
{
	return std::make_shared<  XmlNodeRootAnalyzer >( _mappingsData );
}
