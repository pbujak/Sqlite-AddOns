/*
 * visitors.hpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#pragma once

#include <string>
#include <optional>

namespace sqlw
{

//===============================================================================
struct IRecordGettingVisitor
{
	virtual ~IRecordGettingVisitor() = default;

	virtual void visit( std::string const & _fieldName, std::int64_t _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, double _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::string const & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::optional< std::int64_t > const & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::optional< double > const & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::optional< std::string > const & _value, bool _modifiable ) = 0;
};

//===============================================================================
struct IRecordSettingVisitor
{
	virtual ~IRecordSettingVisitor() = default;

	virtual void visit( std::string const & _fieldName, std::int64_t & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, double & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::string & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::optional< std::int64_t > & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::optional< double > & _value, bool _modifiable ) = 0;

	virtual void visit( std::string const & _fieldName, std::optional< std::string > & _value, bool _modifiable ) = 0;
};

}
