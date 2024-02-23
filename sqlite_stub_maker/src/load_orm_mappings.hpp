/*
 * load_orm_mappings.hpp
 *
 *  Created on: 11 sty 2024
 *      Author: supcio
 */

#pragma once
#include "orm_mappings_data.hpp"
#include <filesystem>


OrmMappingsData loadOrmMappings( std::filesystem::path const & _source );


