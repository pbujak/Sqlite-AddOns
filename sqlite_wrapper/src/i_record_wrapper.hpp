/*
 * i_record_wrapper.hpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#pragma once
#include "visitors.hpp"
#include <vector>
#include <string>

namespace sqlw
{

//===============================================================================
struct IRecordWrapper
{
	virtual ~IRecordWrapper() = default;

	virtual std::int64_t getId() const = 0;

	virtual void setId( std::int64_t _id ) = 0;

	virtual void accept( IRecordGettingVisitor & _visitor ) const = 0;

	virtual void accept( IRecordSettingVisitor & _visitor ) = 0;

	virtual std::vector< std::string > getColumnNames( bool _onlyModifiable ) const = 0;
};

}
