/*
 * resolve_orm_mappings.hpp
 *
 *  Created on: 23 sty 2024
 *      Author: supcio
 */

#pragma once
#include "in_memory_sqlite_database.hpp"
#include "orm_mappings_data.hpp"


void resolveOrmMappings( OrmMappingsData & _ormMappings, InMemorySqliteDatabase const & _database );


