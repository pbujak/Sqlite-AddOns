There are utilities and libraries, that can help use of SQlite in C++ applications. All of them are iinitially written in Linux using Eclipse CDT, but its goal is to be a multiplatform solution (including Visual Studio):

For each project (sqlite_stub_maker, sqlite_wrapper, sqlite_wrapper_tests) there are 2 configurations; Debug and Release.
For both of them a makefile is located in an appropriate subdirectory that means in Debug or Release. So it is enough to run make command:

Examples:
	make
	make clean


