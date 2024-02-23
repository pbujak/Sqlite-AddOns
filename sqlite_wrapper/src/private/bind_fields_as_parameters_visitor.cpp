/*
 * bind_fields_as_parameters_visitor.cpp
 *
 *  Created on: 7 lut 2024
 *      Author: supcio
 */

#include "private/bind_fields_as_parameters_visitor.hpp"
#include "private/bind_parameter.hpp"
#include "private/sqlite_call.hpp"
#include "private/sqlite_handle.hpp"

namespace sqlw
{


//---------------------------------------------------------------------------------------------
BindFieldsAsParametersVisitor::BindFieldsAsParametersVisitor( SqliteHandle< sqlite3_stmt > const & _stmt, bool _onlyModifiable )
	:	m_stmt( _stmt )
	,	m_onlyModifiable( _onlyModifiable )
{
}

//---------------------------------------------------------------------------------------------
BindFieldsAsParametersVisitor::~BindFieldsAsParametersVisitor()
{
}

//---------------------------------------------------------------------------------------------
void
BindFieldsAsParametersVisitor::visit(
		std::string const & _fieldName
	,	std::int64_t _value
	, 	bool _modifiable
	)
{
	if( m_onlyModifiable && !_modifiable )
		return;

	bindParameter( m_stmt, m_index++, _value );
}

//---------------------------------------------------------------------------------------------
void
BindFieldsAsParametersVisitor::visit(
		std::string const & _fieldName
	,	double _value
	, 	bool _modifiable
	)
{
	if( m_onlyModifiable && !_modifiable )
		return;

	bindParameter( m_stmt, m_index++, _value );
}

//---------------------------------------------------------------------------------------------
void
BindFieldsAsParametersVisitor::visit(
		std::string const & _fieldName
	,	std::string const & _value
	, 	bool _modifiable
	)
{
	if( m_onlyModifiable && !_modifiable )
		return;

	bindParameter( m_stmt, m_index++, _value );
}

//---------------------------------------------------------------------------------------------
void
BindFieldsAsParametersVisitor::visit(
		std::string const & _fieldName
	,	std::optional< std::int64_t > const & _value
	, 	bool _modifiable
	)
{
	if( m_onlyModifiable && !_modifiable )
		return;

	bindNullableParameter( m_stmt, m_index++, _value );
}

//---------------------------------------------------------------------------------------------
void
BindFieldsAsParametersVisitor::visit(
		std::string const & _fieldName
	,	std::optional< double > const & _value
	, 	bool _modifiable
	)
{
	if( m_onlyModifiable && !_modifiable )
		return;

	bindNullableParameter( m_stmt, m_index++, _value );
}

//---------------------------------------------------------------------------------------------
void
BindFieldsAsParametersVisitor::visit(
		std::string const & _fieldName
	,	std::optional< std::string > const & _value
	, 	bool _modifiable
	)
{
	if( m_onlyModifiable && !_modifiable )
		return;

	bindNullableParameter( m_stmt, m_index++, _value );
}

//---------------------------------------------------------------------------------------------
int
BindFieldsAsParametersVisitor::getNextBindIndex()
{
	return m_index++;
}

} /* namespace sqlw */

