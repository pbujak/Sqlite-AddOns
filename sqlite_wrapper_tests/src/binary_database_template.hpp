/*
 * binary_database_template.hpp
 *
 *  Created on: 10 lut 2024
 *      Author: supcio
 */

#pragma once
#include "../../sqlite_wrapper/src/i_database_template.hpp"

//=============================================================================
class BinaryDatabaseTemplate : public sqlw::IDatabaseTemplate
{
public:
	BinaryDatabaseTemplate( int _templateId );

	~BinaryDatabaseTemplate();

	Kind getTemplateKind() const override;

	std::string getSqlSchema() const override;

	int getBinaryDataSize() const override;

	void const * const getBinaryData() const override;

private:
	std::pair< void const*, int > m_binaryData;
};

