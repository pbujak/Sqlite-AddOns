/*
 * build_sqlite_database.cpp
 *
 *  Created on: 15 sty 2024
 *      Author: supcio
 */
#include "build_sqlite_database.hpp"
#include "error_handling.hpp"
#include "sql_engine.hpp"
#include "split_sql_into_commands.hpp"
#include "sql_lint.hpp"
#include "util.hpp"


//------------------------------------------------------------------------
InMemorySqliteDatabase
buildSqliteDatabase( SqlScriptData const & _sqlScript )
{
	auto getLineFromOffset = [ & _sqlScript ]( int _offset )
	{
		auto it = _sqlScript.m_lines.upper_bound( _offset );
		--it;

		return std::distance( _sqlScript.m_lines.begin(), it ) + 1;
	};

	InMemorySqliteDatabase inMemoryDatabase;

	SqlCommandOrderedSet sqlCommands = splitSqlIntoCommands( _sqlScript.m_content );

	SqlEngine executor( & inMemoryDatabase );

	SqlLint lint( &inMemoryDatabase, getLineFromOffset );

	for( auto const & sqlCommand : sqlCommands )
	{
		try
		{
			executor.execute( sqlCommand );
		}
		catch( SqlException & e )
		{
			int lineNumber = getLineFromOffset( sqlCommand.m_offset );

			ErrorHandling::throwError(
					ErrorInfo( "SQL", e.getMessage() )
						.setCode( e.getCode() == 1 ? -1 : e.getCode() )
						.setLocation( lineNumber )
				);

		}

		lint.analyze( sqlCommand.m_offset , sqlCommand.m_tokens );
	}

	lint.checkFinal();

	return inMemoryDatabase;
}

