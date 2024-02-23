/*
 * dll_interface.hpp
 *
 *  Created on: 3 lut 2024
 *      Author: supcio
 */

#pragma once

#ifdef __GNUC__

#define DLL_EXPORT __attribute__( ( __visibility__( "default" ) ) )

#endif

#ifdef _WIN32

#ifdef _SQLITE_WRAPPER

#define DLL_EXPORT _declspec( dllexport )

#else

#define DLL_EXPORT _declspec( dllimport )

#endif

#endif
