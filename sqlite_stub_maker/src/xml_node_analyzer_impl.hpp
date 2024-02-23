/*
 * xml_analyzer_impl.hpp
 *
 *  Created on: 12 sty 2024
 *      Author: supcio
 */

#pragma once
#include "xml_node_analyzer.hpp"
#include "orm_mappings_data.hpp"
#include <memory>

std::shared_ptr< IXmlNodeAnalyzer > getRootNodeAnalyzer( OrmMappingsData * const _mappingsData );


