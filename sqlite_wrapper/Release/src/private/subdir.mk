################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/private/bind_fields_as_parameters_visitor.cpp \
../src/private/column_data_provider.cpp \
../src/private/database_impl.cpp \
../src/private/query_impl.cpp \
../src/private/record_iterator_impl.cpp \
../src/private/table_accessor_impl.cpp 

OBJS += \
./src/private/bind_fields_as_parameters_visitor.o \
./src/private/column_data_provider.o \
./src/private/database_impl.o \
./src/private/query_impl.o \
./src/private/record_iterator_impl.o \
./src/private/table_accessor_impl.o 

CPP_DEPS += \
./src/private/bind_fields_as_parameters_visitor.d \
./src/private/column_data_provider.d \
./src/private/database_impl.d \
./src/private/query_impl.d \
./src/private/record_iterator_impl.d \
./src/private/table_accessor_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/private/%.o: ../src/private/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ --std=c++17 -DSQLITE_WRAPPER -I"/home/supcio/workspace/sqlite_wrapper/src" -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


