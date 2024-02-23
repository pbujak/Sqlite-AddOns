################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/db_exception.cpp \
../src/open_database.cpp 

OBJS += \
./src/db_exception.o \
./src/open_database.o 

CPP_DEPS += \
./src/db_exception.d \
./src/open_database.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ --std=c++17 -DSQLITE_WRAPPER -I"/home/supcio/workspace/sqlite_wrapper/src" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


