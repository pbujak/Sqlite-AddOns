/*
 * generate_code.cpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#include "generate_code.hpp"
#include "error_handling.hpp"
#include "code_macro_dictionary.hpp"
#include "run_generation_core.hpp"
#include "resources.hpp"
#include <sstream>
#include <fstream>
#include <typeindex>
#include <typeinfo>

namespace
{

//==========================================================================================
class Generator
{
public:
	Generator( std::filesystem::path const & _target, OrmMappingsData const &  _ormMappings );

	void operator()( CodeMacroDictionary const & _dictionary );

private:
	void generateFile(
			TableData const & _table
		,	std::string const & _file
		, 	int _templateId
		, 	CodeMacroDictionary const & _parentDictionary
		);

	void generateIncludes(
			StringVectorIterator _begin
		,	StringVectorIterator _end
		,	TableData const & _table
		,	std::ofstream & _output
		, 	CodeMacroDictionary const & _dictionary
		);

	void generateForField(
			StringVectorIterator _begin
		,	StringVectorIterator _end
		,	FieldData const & _field
		,	std::ofstream & _output
		, 	CodeMacroDictionary const & _parentDictionary
		);


	std::string buildFileNameNoExt( std::string const & _className );

	StringVector const * const getTemplate( int _resourceId );

private:
	std::filesystem::path const & m_target;

	OrmMappingsData const & m_ormMappings;

	std::unordered_map< int, StringVector > m_mapResourceIdToTemplate;
};

//------------------------------------------------------------------------------------------
Generator::Generator( std::filesystem::path const & _target, OrmMappingsData const &  _ormMappings )
	:	m_target( _target )
	,	m_ormMappings( _ormMappings )
{
}

//------------------------------------------------------------------------------------------
void
Generator::operator()( CodeMacroDictionary const & _dictionary )
{
	for( TableData const & table : m_ormMappings.m_tables )
	{
		std::string fileNameNoExt = buildFileNameNoExt( table.m_cppClass );

		generateFile(
				table
			, 	fileNameNoExt + ".hpp"
			, 	Resources::CodeGenTemplateHeader
			, 	_dictionary
			);
	}
}

//-----------------------------------------------------------------------------------------
std::string
Generator::buildFileNameNoExt( std::string const & _className )
{
	std::string target;

	bool prevUpper = false;

	for( auto it = _className.begin(); it != _className.end(); ++it )
	{
		bool upper = std::isupper( *it );

		if( upper )
		{
			if( !target.empty() && !prevUpper )
				target += "_";
			target += std::tolower( *it );
		}
		else
			target += *it;

		prevUpper = upper;
	}

	return target;
}

//------------------------------------------------------------------------------------------
void
Generator::generateFile(
		TableData const & _table
	,	std::string const & _file
	, 	int _templateId
	,	CodeMacroDictionary const & _parentDictionary
	)
{
	auto toString = []( bool _value )
		{
			return _value ? "true" : "false";
		};

	StringVector const * const templ = getTemplate( _templateId );

	CodeMacroDictionary dictionary( &_parentDictionary );
	dictionary.setMacro( "TableName", _table.m_name );
	dictionary.setMacro( "RecordClass", _table.m_cppClass );
	dictionary.setMacro( "IsView", toString( _table.m_isView ) );
	dictionary.setMacro( "TableModifiable", toString( _table.m_modifiable ) );

	std::filesystem::path path = m_target / _file;

	std::ofstream output( path.string() );

	auto forFields = [ this, &output, &_table, &dictionary ](
			StringVectorIterator _it
		, 	StringVectorIterator _end
		)
	{
		for( FieldData const & field : _table.m_resolvedFields )
			generateForField( _it, _end, field, output, dictionary );
	};

	auto forIncludes = [ this, &output, &_table, &dictionary ](
			StringVectorIterator _it
		, 	StringVectorIterator _end
		)
	{
		generateIncludes( _it, _end, _table, output, dictionary );
	};

	std::unordered_map< std::string, ForEachFunction > forEachHandlers
		=	{
					{ "include", std::move( forIncludes ) }
				,	{ "field", std::move( forFields ) }
			};

	runGenerationCore(
			templ->begin()
		,	templ->end()
		,	dictionary
		,	forEachHandlers
		,	output
		);
}

//------------------------------------------------------------------------------------------
void
Generator::generateIncludes(
		StringVectorIterator _begin
	,	StringVectorIterator _end
	,	TableData const & _table
	,	std::ofstream & _output
	, 	CodeMacroDictionary const & _dictionary
	)
{
	std::set< std::string > includes;

	for( FieldData const & _field : _table.m_resolvedFields )
	{
		if( !_field.m_notNull )
			includes.insert( "<optional>" );

		if( _field.m_cppType == &typeid( std::string ) )
			includes.insert( "<string>" );
	}

	for( auto const & include : includes )
	{
		CodeMacroDictionary scopeDictionary( &_dictionary );
		scopeDictionary.setMacro( "Include", include );

		runGenerationCore( _begin, _end, scopeDictionary, {}, _output );
	}
}

//------------------------------------------------------------------------------------------
void
Generator::generateForField(
		StringVectorIterator _begin
	,	StringVectorIterator _end
	,	FieldData const & _field
	,	std::ofstream & _output
	, 	CodeMacroDictionary const & _parentDictionary
	)
{
	CodeMacroDictionary dictionary( &_parentDictionary );
	dictionary.setMacro( "ColumnName", _field.m_name );
	dictionary.setMacro( "cppFieldName", _field.m_cppMember );
	dictionary.setMacro( "ColumnModifiable", _field.m_modifiable ? "true" : "false" );

	dictionary.setMacro( "cppFieldMove", "move" );

	if( _field.m_notNull )
	{
		if( _field.m_cppType == &typeid( std::int64_t ) )
		{
			dictionary.setMacro( "cppFieldInit", " = 0" );
			dictionary.setMacro( "cppFieldExchangeArg", ", 0" );
			dictionary.setMacro( "cppFieldMove", "exchange" );
		}
		else if( _field.m_cppType == &typeid( double ) )
		{
			dictionary.setMacro( "cppFieldInit", " = 0.0" );
			dictionary.setMacro( "cppFieldExchangeArg", ", 0.0" );
			dictionary.setMacro( "cppFieldMove", "exchange" );
		}
	}

	auto getType = [ & _field ]() -> std::string
		{
			static std::unordered_map< std::type_index, std::string > s_mapTypeIdToCppName
				=	{
							{ std::type_index( typeid( std::int64_t ) ), "std::int64_t" }
						,	{ std::type_index( typeid( double) ), "double" }
						,	{ std::type_index( typeid( std::string) ), "std::string" }
					};

			auto it = s_mapTypeIdToCppName.find( std::type_index( *_field.m_cppType ) );
			if( it == s_mapTypeIdToCppName.end() )
				return {};

			std::string type = it->second;

			if( !_field.m_notNull )
				type = "std::optional< " + type + " >";

			return type;
		};

	dictionary.setMacro( "cppFieldType", std::move( getType ) );

	runGenerationCore( _begin, _end, dictionary, {}, _output );
}

//------------------------------------------------------------------------------------------
StringVector const * const
Generator::getTemplate( int _resourceId )
{
	auto it = m_mapResourceIdToTemplate.find( _resourceId );
	if( it == m_mapResourceIdToTemplate.end() )
	{
		std::string templ = Resources::getString(_resourceId );
		std::istringstream iss( templ );

		StringVector lines;

		std::string line;

		while( std::getline( iss, line ) )
			lines.push_back( std::move( line ) );

		m_mapResourceIdToTemplate[ _resourceId ] = std::move( lines );
		it = m_mapResourceIdToTemplate.find( _resourceId );
	}

	return &it->second;
}

}

//------------------------------------------------------------------------------------------
void
generateCode( std::filesystem::path const &  _target, OrmMappingsData const & _ormMappings )
{
	if( _ormMappings.m_sourceXmlFile.empty() )
		return;

	std::error_code error;

	if(
			!std::filesystem::is_directory( _target )
		&&	!std::filesystem::create_directories( _target, error )
		)
	{
		ScopedContextFile scf( _target.string() );

		ErrorHandling::throwError(
				ErrorInfo( "FATAL", "Cannot create directory for generated code." )
			);
	}

	CodeMacroDictionary dictionary;
	dictionary.setMacro(
			"Namespace"
		, 	_ormMappings.m_namespace.empty() ? "Database" : _ormMappings.m_namespace
		);

	Generator generator( _target, _ormMappings );
	generator( dictionary );
}



