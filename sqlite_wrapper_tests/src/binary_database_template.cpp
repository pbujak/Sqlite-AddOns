/*
 * binary_database_template.cpp
 *
 *  Created on: 10 lut 2024
 *      Author: supcio
 */

#include "binary_database_template.hpp"
#include "resources.hpp"

//-------------------------------------------------------------------------
BinaryDatabaseTemplate::BinaryDatabaseTemplate( int _templateId )
{
	m_binaryData = Resources::getBinary( _templateId );

}

//-------------------------------------------------------------------------
BinaryDatabaseTemplate::~BinaryDatabaseTemplate()
{
}

//-------------------------------------------------------------------------
BinaryDatabaseTemplate::Kind
BinaryDatabaseTemplate::getTemplateKind() const
{
	return BinaryDatabaseTemplate::Kind::Binary;
}

//-------------------------------------------------------------------------
std::string
BinaryDatabaseTemplate::getSqlSchema() const
{
	return {};
}

//-------------------------------------------------------------------------
int
BinaryDatabaseTemplate::getBinaryDataSize() const
{
	return m_binaryData.second;
}

//-------------------------------------------------------------------------
void const * const
BinaryDatabaseTemplate::getBinaryData() const
{
	return m_binaryData.first;
}

