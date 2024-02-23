/*
 * sql_lint.hpp
 *
 *  Created on: 16 sty 2024
 *      Author: supcio
 */

#pragma once
#include "sql_token.hpp"
#include "sql_analyzer.hpp"
#include "db_objects.hpp"
#include "in_memory_sqlite_database.hpp"
#include "string_key.hpp"
#include <functional>
#include <vector>
#include <set>
#include <unordered_set>

//=========================================================================
class SqlLint
{
public:
	SqlLint( InMemorySqliteDatabase * _database, std::function< int( int ) > const & _getLineFromOffset );

	void analyze( int _offset, std::vector< SqlToken > const & _sqlCommand );

	void checkFinal() const;

private:
	struct UntouchedTrigger
	{
		int m_offset;

		DbParsedTrigger m_trigger;

		inline std::string getTable() const
		{
			return m_trigger.m_table;
		}
	};


private:
	void checkUntouchedTriggers() const;

	void checkIncompleteTriggers() const;

	void checkForeignKeyIndexes() const;

	void checkNotEmptyTables() const;

	bool analyzeInsert( int _offset, SqlAnalyzer & _analyzer );

	bool analyzeDelete( int _offset, SqlAnalyzer & _analyzer );

	bool analyzeUpdate( int _offset, SqlAnalyzer & _analyzer );

	void analyzeCreate( int _offset, SqlAnalyzer & _analyzer );

	void analyzeView( int _offset, std::vector< SqlToken > const & _sqlCommand );

	bool analyzeTrigger( int _offset, SqlAnalyzer & _analyzer );

	bool analyzeDropTrigger( int _offset, SqlAnalyzer & _analyzer );

	void skipWithRecursive( int _offset, SqlAnalyzer & _analyzer );

	bool isCommandRegardingWith( SqlAnalyzer & _analyzer, std::string const & _command );

	template< typename ... _MessageFragments >
	void warning( int _offset, _MessageFragments const & ... _messageFragments ) const;

private:
	InMemorySqliteDatabase * m_database;

	std::function< int( int ) > m_getLineFromOffset;

	std::multiset< UntouchedTrigger, CompareByMethod< &UntouchedTrigger::getTable > > m_mapTableToUntestedTrigger;
};


