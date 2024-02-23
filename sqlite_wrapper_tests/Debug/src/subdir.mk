################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/binary_database_template.cpp \
../src/main.cpp \
../src/resources.cpp \
../src/test_base.cpp \
../src/test_open.cpp \
../src/test_query.cpp \
../src/test_records.cpp 

OBJS += \
./src/binary_database_template.o \
./src/main.o \
./src/resources.o \
./src/test_base.o \
./src/test_open.o \
./src/test_query.o \
./src/test_records.o 

CPP_DEPS += \
./src/binary_database_template.d \
./src/main.d \
./src/resources.d \
./src/test_base.d \
./src/test_open.d \
./src/test_query.d \
./src/test_records.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


