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

#include "gen/dbtemplate_db3_i.hpp"

//==================================================
struct ResourceEntry
{
	unsigned char *m_data;

	unsigned int m_length;
};


std::unordered_map< int, ResourceEntry > s_entries
	=	{
				{ Db3Template, { dbtemplate_db3, dbtemplate_db3_len } }
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

//---------------------------------------------------------------------------
std::pair< void const*, int > getBinary( int _id )
{
	auto it = s_entries.find( _id );

	if( it == s_entries.end() )
		return {};

	return {
				reinterpret_cast< char const* >( it->second.m_data )
			, 	it->second.m_length
			};
}

}
