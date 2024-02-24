################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/arguments.cpp \
../src/build_sqlite_database.cpp \
../src/code_macro_dictionary.cpp \
../src/db_auxiliary.cpp \
../src/db_object_extract.cpp \
../src/db_schema_object_cache.cpp \
../src/error_handling.cpp \
../src/execute_assertion.cpp \
../src/generate_code.cpp \
../src/in_memory_sqlite_database.cpp \
../src/load_orm_mappings.cpp \
../src/load_sql_script.cpp \
../src/orm_mappings_data.cpp \
../src/resolve_orm_mappings.cpp \
../src/resources.cpp \
../src/run_generation_core.cpp \
../src/split_sql_into_commands.cpp \
../src/sql_analyzer.cpp \
../src/sql_engine.cpp \
../src/sql_lint.cpp \
../src/sql_string_iterator.cpp \
../src/sql_tokenizer.cpp \
../src/sqlite_stub_maker.cpp \
../src/util.cpp \
../src/xml_node_analyzer.cpp \
../src/xml_node_analyzer_impl.cpp 

OBJS += \
./src/arguments.o \
./src/build_sqlite_database.o \
./src/code_macro_dictionary.o \
./src/db_auxiliary.o \
./src/db_object_extract.o \
./src/db_schema_object_cache.o \
./src/error_handling.o \
./src/execute_assertion.o \
./src/generate_code.o \
./src/in_memory_sqlite_database.o \
./src/load_orm_mappings.o \
./src/load_sql_script.o \
./src/orm_mappings_data.o \
./src/resolve_orm_mappings.o \
./src/resources.o \
./src/run_generation_core.o \
./src/split_sql_into_commands.o \
./src/sql_analyzer.o \
./src/sql_engine.o \
./src/sql_lint.o \
./src/sql_string_iterator.o \
./src/sql_tokenizer.o \
./src/sqlite_stub_maker.o \
./src/util.o \
./src/xml_node_analyzer.o \
./src/xml_node_analyzer_impl.o 

CPP_DEPS += \
./src/arguments.d \
./src/build_sqlite_database.d \
./src/code_macro_dictionary.d \
./src/db_auxiliary.d \
./src/db_object_extract.d \
./src/db_schema_object_cache.d \
./src/error_handling.d \
./src/execute_assertion.d \
./src/generate_code.d \
./src/in_memory_sqlite_database.d \
./src/load_orm_mappings.d \
./src/load_sql_script.d \
./src/orm_mappings_data.d \
./src/resolve_orm_mappings.d \
./src/resources.d \
./src/run_generation_core.d \
./src/split_sql_into_commands.d \
./src/sql_analyzer.d \
./src/sql_engine.d \
./src/sql_lint.d \
./src/sql_string_iterator.d \
./src/sql_tokenizer.d \
./src/sqlite_stub_maker.d \
./src/util.d \
./src/xml_node_analyzer.d \
./src/xml_node_analyzer_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ --std=c++17 -I/usr/include/libxml2 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


