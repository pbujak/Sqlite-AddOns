There are utilities and libraries, that can help use of SQlite in C++ applications. All of them are iinitially written 
in Linux using Eclipse CDT, but its goal is to be a multiplatform solution (including Visual Studio):
1) sqlite_stub_maker: generates stub SQlite database and C++ ORM wrappers code for tables and views. As input is used a SQL script with DDL commands related to database schema, but first line should look like "--#include-orm-mappings "orm-mappings.xml"
2) sqlite_wrapper - library between applications and SQLite (now shared library for Linux, but could be also used as a DLL in Windows)
3) sqlite_wrapper_tests - unit tests for library sqlite_wrapper
