/*
 * i_database_template.hpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#pragma once
#include <string>

namespace sqlw
{

//===========================================================================
struct IDatabaseTemplate
{
	enum class Kind
	{
		Binary, SqlSchema
	};

	virtual ~IDatabaseTemplate() = default;

	virtual Kind getTemplateKind() const = 0;

	virtual std::string getSqlSchema() const = 0;

	virtual int getBinaryDataSize() const = 0;

	virtual void const * const getBinaryData() const = 0;
};

}
