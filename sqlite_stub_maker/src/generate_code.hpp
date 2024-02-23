/*
 * generate_code.hpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#pragma once
#include "orm_mappings_data.hpp"


void generateCode( std::filesystem::path const &  _target, OrmMappingsData const &  _ormMappings );

