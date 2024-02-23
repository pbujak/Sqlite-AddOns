#pragma once
#include <filesystem>

struct Arguments
{
	std::filesystem::path m_sqlFile;
	std::filesystem::path m_dbOutputFile;
	std::filesystem::path m_generateCodeDirectory;
};

Arguments parseArguments( int argc, char *argv[] );
