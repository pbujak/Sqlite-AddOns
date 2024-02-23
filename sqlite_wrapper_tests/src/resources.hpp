/*
 * resources.hpp
 *
 *  Created on: 30 sty 2024
 *      Author: supcio
 */

#pragma once
#include <string>


namespace Resources
{

int constexpr Db3Template = 1;


std::string getString( int _id );

std::pair< void const*, int > getBinary( int _id );

}

