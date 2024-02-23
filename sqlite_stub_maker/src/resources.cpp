/*
 * resources.cpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#include "resources.hpp"
#include <unordered_map>

namespace Resources
{

namespace
{

#include "gen/code_gen_template_i.hpp"

//==================================================
struct ResourceEntry
{
	unsigned char *m_data;

	unsigned int m_length;
};


std::unordered_map< int, ResourceEntry > s_entries
	=	{
				{ CodeGenTemplateHeader, { code_gen_template_hpp, code_gen_template_hpp_len } }
		};

}

//---------------------------------------------------------------------------
std::string getString( int _id )
{
	auto it = s_entries.find( _id );

	if( it == s_entries.end() )
		return {};

	return std::string(
				reinterpret_cast< char const* >( it->second.m_data )
			, 	it->second.m_length
		);
}

}
