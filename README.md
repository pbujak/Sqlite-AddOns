** SQLite stub generator including database and C++ code and C++ library that can use generated code **

There are utilities and libraries, that can help use of SQlite in C++ applications. All of them are iinitially written in Linux using Eclipse CDT, but its goal is to be a multiplatform solution (including Visual Studio):

For each project (sqlite_stub_maker, sqlite_wrapper, sqlite_wrapper_tests) there are 2 configurations; Debug and Release.
For both of them a makefile is located in an appropriate subdirectory that means in Debug or Release. So it is enough to run make command:

Examples:
	make
	make clean

Additionally in case of sqlite_stub_maker the C++ metasource template is located in "resources" subdirectory, in Linux it is translated by xxd into "gen" subdirectory (it is handled by "prebuild.sh" script before the actual build in CDT, but it is not included in makefile). In Windows it would be managed differently by Microsoft's resource management system.
