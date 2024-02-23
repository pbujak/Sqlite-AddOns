//============================================================================
// Name        : sqlite_stub_maker.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "arguments.hpp"
#include "load_sql_script.hpp"
#include "load_orm_mappings.hpp"
#include "resolve_orm_mappings.hpp"
#include "build_sqlite_database.hpp"
#include "error_handling.hpp"
#include "generate_code.hpp"
#include <iostream>
#include <optional>

using namespace std;

namespace
{

//--------------------------------------------------------------------------------------
[[ noreturn ]]
void
throwFatalError( std::string const & _errorMessage )
{
	ErrorHandling::throwError( ErrorInfo( "FATAL", _errorMessage ) );
}

//--------------------------------------------------------------------------------------
void
checkOutputPath( std::filesystem::path const & _outputPath )
{
	if( _outputPath.empty() )
		return;

	ScopedContextFile scf( _outputPath.string() );

	if( std::filesystem::is_directory( _outputPath ) )
		throwFatalError( "Directory of the same name exists." );

	if( !std::filesystem::exists( _outputPath ) )
		return;

	if( access( _outputPath.string().c_str(), W_OK ) != 0 )
		throwFatalError( "Cannot write to file." );
}

//--------------------------------------------------------------------------------------
void
checkCodeGenerationDirectory( std::filesystem::path const & _codeGenDir )
{
	if( _codeGenDir.empty() )
		return;

	ScopedContextFile scf( _codeGenDir.string() );

	if( std::filesystem::is_directory( _codeGenDir ) )
		return;

	if( std::filesystem::is_regular_file( _codeGenDir ) )
		throwFatalError( "File of the name exists, so cannot be treated as a directory for generated code." );

	auto parent = _codeGenDir.parent_path();

	if( !std::filesystem::is_directory( parent ) )
		throwFatalError( "Cannot find directory " + parent.string() );
}

}


//--------------------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
	Arguments arguments = parseArguments( argc, argv );

	if( arguments.m_sqlFile.empty() )
	{
		std::cout
			<< "Usage: sqlite_stub_maker source.sql [OPTIONS]" << std::endl
			<< std::endl
			<< "Generates stub C++ code and stub Sqlite database from SQL script." << std::endl
			<< "   -o DBFILE    Output file DBFILE with Sqlite stub database as template." << std::endl
			<< "   -c DIR       Target directory DIR where generated C++ code is placed." << std::endl
			<< std::endl
			<< "The SQL script syntax supports additional features to test triggers." << std::endl
			<< "   Assertions       \"ASSERT SELECT MAX(id) FROM demo = 10;\"" << std::endl
			<< "   Transact-SQL like variables without declarations" << std::endl
			<< "                    \"SET @Variable = SELECT COUNT(*) FROM demo1;\"" << std::endl
			<< std::endl;
		return -1;
	}

	ScopedContextFile scf( arguments.m_sqlFile.string() );

	try
	{
		checkOutputPath( arguments.m_dbOutputFile );
		checkCodeGenerationDirectory( arguments.m_generateCodeDirectory );

		SqlScriptData sql = loadSqlScript( arguments.m_sqlFile );

		OrmMappingsData ormMappings;

		if( !sql.m_xmlOrmMappings.empty() )
			ormMappings = loadOrmMappings(  sql.m_xmlOrmMappings );
		else
		{
			ErrorHandling::reportWarning(
					ErrorInfo(
							"ORM"
						, 	"Missing object-relation mapping file, it should be included from SQL using --#include-orm-mapping macro."
						)
				);
		}

		InMemorySqliteDatabase inMemoryDatabase = buildSqliteDatabase( sql );

		resolveOrmMappings( ormMappings, inMemoryDatabase );

		if( !arguments.m_dbOutputFile.empty() )
		{
			std::string outputError;

			if( !inMemoryDatabase.dump( arguments.m_dbOutputFile, outputError ) )
			{
				ScopedContextFile scf( arguments.m_dbOutputFile.string() );
				ErrorHandling::throwError( ErrorInfo( "FATAL", outputError ) );
			}
		}

		if( !arguments.m_generateCodeDirectory.empty() )
			generateCode( arguments.m_generateCodeDirectory, ormMappings );
	}
	catch( std::exception & e )
	{
		ErrorHandling::printMessages();
		return -1;
	}

	ErrorHandling::printMessages();
	return 0;
}
