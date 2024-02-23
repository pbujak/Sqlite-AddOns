/*
 * orm_mappings_data.hpp
 *
 *  Created on: 9 sty 2024
 *      Author: supcio
 */
#pragma once
#include "string_key.hpp"
#include <string>
#include <set>
#include <vector>
#include <boost/logic/tribool.hpp>

//=================================================================
struct FieldData
{
	int m_xmlOrmLine = 0;

	std::string m_name;
	std::string m_cppMember;
	std::type_info const *m_cppType = nullptr;

	boost::tribool m_notNull = boost::indeterminate;

	bool m_modifiable = true;
};

//=================================================================
struct TableData
{
	int m_xmlOrmLine = 0;

	std::string m_name;
	std::string m_cppClass;

	bool m_isView = false;

	bool m_modifiable = true;

	std::set< FieldData, CompareByField< &FieldData::m_name > > m_xmlRawFields;

	std::vector< FieldData > m_resolvedFields;
};

//=================================================================
struct OrmMappingsData
{
	std::string m_sourceXmlFile;
	std::string m_namespace;

	std::vector< TableData > m_tables;
};



