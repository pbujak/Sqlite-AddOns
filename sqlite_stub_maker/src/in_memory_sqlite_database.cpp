/*
 * in_memory_sqlite_database.cpp
 *
 *  Created on: 15 sty 2024
 *      Author: supcio
 */

#include "in_memory_sqlite_database.hpp"
#include "sqlite_handle.hpp"
#include <assert.h>
#include <type_traits>

namespace
{

//=======================================================
template < typename _Container, typename = std::void_t<> >
struct HasInsert
{
	static constexpr bool value = false;
};

//=======================================================
template < typename _Container >
struct HasInsert<
		_Container
	, 	std::void_t<
			decltype( std::declval< _Container >().insert( std::declval< typename _Container::value_type >() ) ) >
			>
{
	static constexpr bool value = true;
};

//-----------------------------------------------------------------------
template< typename _Parameter, typename ... _More >
inline void
bindParametersImpl(
		sqlite3_stmt * const _stmt
	,	int _index
	,	_Parameter const & _parameter
	, 	_More const & ... _more
	)
{
	if constexpr( std::is_convertible_v< _Parameter, std::string > )
	{
		sqlite3_bind_text(
				_stmt
			, 	_index
			, 	_parameter.c_str()
			, 	-1
			,	SQLITE_TRANSIENT
			);
	}
	else if constexpr ( std::is_convertible_v< _Parameter, int > )
		sqlite3_bind_int( _stmt, _index, _parameter );

	else if constexpr ( std::is_convertible_v< _Parameter, double > )
		sqlite3_bind_double( _stmt, _index, _parameter );

	if constexpr ( sizeof...( _More ) > 0 )
		bindParametersImpl( _stmt, _index + 1, _more ... );
}

//-----------------------------------------------------------------------
template< typename ... _BindParameters >
void
bindParameters( sqlite3_stmt * const stmt, _BindParameters const & ... _parameters )
{
	if constexpr ( sizeof...( _BindParameters ) > 0 )
		bindParametersImpl( stmt, 1, _parameters ... );
}

//-----------------------------------------------------------------------
std::string
loadString( sqlite3_stmt * const _stmt, int _index )
{
	if( sqlite3_column_type( _stmt, _index ) == SQLITE_NULL )
		return {};

	return reinterpret_cast< char const* >( sqlite3_column_text( _stmt, _index ) );
}

//-----------------------------------------------------------------------
DbSchemaObject
loadObject( sqlite3_stmt * const _stmt )
{
	static std::unordered_map< std::string, DbSchemaObject::Kind > mapKind
		=	{
					{ "table", DbSchemaObject::Kind::Table }
				,	{ "view", DbSchemaObject::Kind::View }
				,	{ "index", DbSchemaObject::Kind::Index }
				,	{ "trigger", DbSchemaObject::Kind::Trigger }
			};


	DbSchemaObject result;

	std::string kind = loadString( _stmt, 0 );

	result.m_kind = mapKind.find( kind )->second;
	result.m_name = loadString( _stmt, 1 );
	result.m_relatedTable = loadString( _stmt, 2 );
	result.m_rootPage = sqlite3_column_int( _stmt, 3 );
	result.m_sql = loadString( _stmt, 4 );

	return result;
}

}

//-----------------------------------------------------------------------
sqlite3 * const
InMemorySqliteDatabase::ensureConnection()
{
	if( !m_connection )
	{
		if( sqlite3_open( ":memory:", &m_connection ) != SQLITE_OK )
			return nullptr;

		sqlite3_extended_result_codes( m_connection, true );
	}
	return m_connection;
}

//------------------------------------------------------------------------------
void
InMemorySqliteDatabase::execute(
		std::string const & _sql
	,	std::unordered_map< std::string, SqlValue > const & _variables
	,	std::function< void( sqlite3_stmt * const ) > const & _rowCallback
	)
{
	auto throwError = []( sqlite3 * const _connection )
	{
		throw SqlException(
				sqlite3_errmsg( _connection )
			,	sqlite3_extended_errcode( _connection )
			);
	};

	auto db = ensureConnection();
	if( !db )
		throw SqlException( "Not enough memory to initialize connection.", SQLITE_ERROR );

	SqliteStatement stmt;

	if( sqlite3_prepare_v2( db, _sql.c_str(), -1, &stmt, nullptr ) != SQLITE_OK )
		throwError( db );

	for( auto const & _variablAndValue : _variables )
	{
		int index = sqlite3_bind_parameter_index( stmt, _variablAndValue.first.c_str() );

		assert( index != -1 );

		sqlite3_bind_value( stmt, index, _variablAndValue.second );
	}

	int result = sqlite3_step( stmt );

	while( result == SQLITE_ROW )
	{
		_rowCallback( stmt );

		result = sqlite3_step( stmt );
	}

	if( result == SQLITE_ERROR )
		throwError( db );
}

//------------------------------------------------------------------------------
SqlValue
InMemorySqliteDatabase::executeWithResult(
		std::string const & _sql
	,	std::unordered_map< std::string, SqlValue > const & _variables
	)
{
	SqlValue result;

	auto throwNoScalar = [ _sql ]()
		{
			throw SqlException( "Expected scalar result of statement " + Util::quote( _sql ) );
		};

	auto getResult = [ &result, &throwNoScalar ]( sqlite3_stmt * const _stmt )
		{
			if( !result.empty() )
				throwNoScalar();

			if( sqlite3_column_count( _stmt ) != 1 )
				throwNoScalar();

			result = sqlite3_column_value( _stmt, 0 );
		};

	execute( _sql, _variables, getResult );

	return result;
}

//-----------------------------------------------------------------------
sqlite3_stmt *const
InMemorySqliteDatabase::getPreparedStatement( std::string const & _sql ) const
{
	auto it = m_preparedStatementPool.find( _sql );

	if( it != m_preparedStatementPool.end() )
	{
		sqlite3_clear_bindings( it->second );
		sqlite3_reset( it->second );

		return it->second;
	}

	sqlite3_stmt *newStmt = nullptr;

	if( sqlite3_prepare_v2( m_connection, _sql.c_str(), _sql.size(), &newStmt, nullptr ) != SQLITE_OK )
	{
		std::string error = sqlite3_errmsg( m_connection );
		return nullptr;
	}

	m_preparedStatementPool[ _sql ] = SqliteStatement( newStmt );

	return newStmt;
}

//-----------------------------------------------------------------------
DbSchemaObjectSet
InMemorySqliteDatabase::getTablesOrViews() const
{
	DbSchemaObjectSet result;

	getItems(
			result
		,	"select * from sqlite_master where type in ('table','view')"
		,	loadObject
		);

	return result;
}

//-----------------------------------------------------------------------
DbSchemaObjectSet
InMemorySqliteDatabase::getTriggers() const
{
	DbSchemaObjectSet result;

	getItems(
			result
		,	"select * from sqlite_master where type = 'trigger'"
		,	loadObject
		);

	return result;
}

//-----------------------------------------------------------------------
DbTableInfo
InMemorySqliteDatabase::getTableInfo( std::string const & _table ) const
{
	auto loadTableInfo = []( sqlite3_stmt * const _stmt )
	{
		DbTableColumn result;
		result.m_name = loadString( _stmt, 1 );
		result.m_type = loadString( _stmt, 2 );
		result.m_notNull = sqlite3_column_int( _stmt, 3 ) > 0;
		result.m_default = loadString( _stmt, 4 );
		result.m_primaryKey = sqlite3_column_int( _stmt, 5 ) > 0;
		return result;
	};

	DbTableInfo result;

	getItems(
			result
		,	"SELECT * from pragma_table_info( ? )"
		,	loadTableInfo
		,	_table
		);

	return result;
}

//-----------------------------------------------------------------------
DbForeignKeyList
InMemorySqliteDatabase::getForeignKeyList( std::string const & _table ) const
{
	auto loadForeignKey = []( sqlite3_stmt * const _stmt )
	{
		DbForeignKey result;
		result.m_tableForeign = loadString( _stmt, 2 );
		result.m_columnLocal = loadString( _stmt, 3 );
		result.m_columnForeign = loadString( _stmt, 4 );
		return result;
	};

	DbForeignKeyList result;

	getItems(
			result
		,	"SELECT * from pragma_foreign_key_list( ? )"
		,	loadForeignKey
		,	_table
		);

	return result;
}

//-----------------------------------------------------------------------
DbIndexInfo
InMemorySqliteDatabase::getIndexInfo( std::string const & _indexName ) const
{
	auto loadIndexColumn = []( sqlite3_stmt * const _stmt )
	{
		DbIndexedColumn result;
		result.m_columnRankWithinIndex = sqlite3_column_int( _stmt, 0 );
		result.m_columnRankWithinTable = sqlite3_column_int( _stmt, 1 );

		if( sqlite3_column_type( _stmt, 2 ) != SQLITE_NULL )
			result.m_columnName = loadString( _stmt, 2 );

		return result;
	};


	DbIndexInfo result;

	getItems(
			result
		,	"SELECT * from pragma_index_info( ? )"
		,	loadIndexColumn
		,	_indexName
		);

	return result;
}

//-----------------------------------------------------------------------
DbIndexList
InMemorySqliteDatabase::getIndexList( std::string const & _table ) const
{
	auto loadIndex = []( sqlite3_stmt * const _stmt )
	{
		DbIndex result;
		result.m_name = loadString( _stmt, 1 );
		result.m_unique = sqlite3_column_int( _stmt, 2 ) > 0;
		result.m_kind = loadString( _stmt, 3 );
		result.m_partial = sqlite3_column_int( _stmt, 4 ) > 0;
		return result;
	};

	DbIndexList result;

	getItems(
			result
		,	"SELECT * from pragma_index_list( ? )"
		,	loadIndex
		,	_table
		);

	return result;
}

//-----------------------------------------------------------------------
int
InMemorySqliteDatabase::getTableRowCount( std::string const & _table ) const
{
	auto getCount = []( sqlite3_stmt * const _stmt )
	{
		return sqlite3_column_int( _stmt, 0 );
	};

	std::vector< int > numbers;

	getItems(
			numbers
		,	"SELECT count(*) from " + _table
		,	getCount
		,	_table
		);

	assert( numbers.size() == 1 );

	return numbers[ 0 ];
}

//-----------------------------------------------------------------------
template< typename _Container, typename _CreateItem, typename ... _BindParameters >
void
InMemorySqliteDatabase::getItems(
		_Container & _container
	, 	std::string const & _sql
	,	_CreateItem const & _createItem
	, 	_BindParameters const & ... _parameters
	) const
{
	using Item = decltype(
		std::declval< _CreateItem const & >()( ( sqlite3_stmt* const )nullptr )
		);

	sqlite3_stmt *const stmt = getPreparedStatement( _sql );

	bindParameters( stmt, _parameters ... );

	while( sqlite3_step( stmt ) == SQLITE_ROW )
	{
		Item newItem = _createItem( stmt );

		if constexpr ( HasInsert< _Container >::value )
			_container.insert( std::move( newItem ) );
		else
			_container.push_back( std::move( newItem ) );
	}
}

//-----------------------------------------------------------------------
bool
InMemorySqliteDatabase::dump( std::filesystem::path const & _targetPath, std::string & _errorMessage ) const
{
	auto setError = [ &_errorMessage ]( std::string const & _error )
		{
			_errorMessage = _error;
			return false;
		};

	SqliteDb targetDb;

	if( sqlite3_open( _targetPath.string().c_str(), &targetDb ) != SQLITE_OK )
		return setError( "Cannot create file " + Util::quote( _targetPath.string() ) );

	{
		SqliteHandle< sqlite3_backup > backup
			= sqlite3_backup_init( targetDb, "main", m_connection, "main" );

		if( !backup )
			return setError( sqlite3_errmsg( targetDb ) );

		if( sqlite3_backup_step( backup, -1 ) != SQLITE_DONE )
			return setError( sqlite3_errmsg( targetDb ) );
	}

	return true;
}
