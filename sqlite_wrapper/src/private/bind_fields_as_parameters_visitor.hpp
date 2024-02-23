/*
 * bind_fields_as_parameters_visitor.hpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#pragma once

#include "private/sqlite_handle.hpp"
#include "visitors.hpp"

namespace sqlw
{

//======================================================================
class BindFieldsAsParametersVisitor: public IRecordGettingVisitor
{
public:
	BindFieldsAsParametersVisitor( SqliteHandle< sqlite3_stmt > const & _stmt, bool _onlyModifiable );

	~BindFieldsAsParametersVisitor();

	void visit( std::string const & _fieldName, std::int64_t _value, bool _modifiable ) override;

	void visit( std::string const & _fieldName, double _value, bool _modifiable ) override;

	void visit( std::string const & _fieldName, std::string const & _value, bool _modifiable ) override;

	void visit( std::string const & _fieldName, std::optional< std::int64_t > const & _value, bool _modifiable ) override;

	void visit( std::string const & _fieldName, std::optional< double > const & _value, bool _modifiable ) override;

	void visit( std::string const & _fieldName, std::optional< std::string > const & _value, bool _modifiable ) override;

	int getNextBindIndex();

private:
	SqliteHandle< sqlite3_stmt > m_stmt;

	bool m_onlyModifiable;

	int m_index = 1;
};

} /* namespace sqlw */
