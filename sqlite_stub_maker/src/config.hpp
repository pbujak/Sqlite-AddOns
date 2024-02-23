/*
 * config.hpp
 *
 *  Created on: 9 sty 2024
 *      Author: supcio
 */

#pragma once

enum class PlatformType
{
		Windows
	,	Linux
	,	Unix
};

#ifdef _WIN32

PlatformType constexpr Platform = PlatformType::Windows;

#elif __linux__

PlatformType constexpr Platform = PlatformType::Linux;

#elif __unix__

PlatformType constexpr Platform = PlatformType::Unix;

#endif


