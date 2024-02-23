#pragma once
#include "in_memory_sqlite_database.hpp"
#include "load_sql_script.hpp"
#include "db_auxiliary.hpp"

InMemorySqliteDatabase buildSqliteDatabase( SqlScriptData const & _sqlScript );
