################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/lib/dbcc/2bsm.c \
../src/lib/DPS/lib/dbcc/2c.c \
../src/lib/DPS/lib/dbcc/2csv.c \
../src/lib/DPS/lib/dbcc/2json.c \
../src/lib/DPS/lib/dbcc/2xml.c \
../src/lib/DPS/lib/dbcc/can.c \
../src/lib/DPS/lib/dbcc/getopt.c \
../src/lib/DPS/lib/dbcc/main.c \
../src/lib/DPS/lib/dbcc/mpc.c \
../src/lib/DPS/lib/dbcc/parse.c \
../src/lib/DPS/lib/dbcc/util.c 

C_DEPS += \
./src/lib/DPS/lib/dbcc/2bsm.d \
./src/lib/DPS/lib/dbcc/2c.d \
./src/lib/DPS/lib/dbcc/2csv.d \
./src/lib/DPS/lib/dbcc/2json.d \
./src/lib/DPS/lib/dbcc/2xml.d \
./src/lib/DPS/lib/dbcc/can.d \
./src/lib/DPS/lib/dbcc/getopt.d \
./src/lib/DPS/lib/dbcc/main.d \
./src/lib/DPS/lib/dbcc/mpc.d \
./src/lib/DPS/lib/dbcc/parse.d \
./src/lib/DPS/lib/dbcc/util.d 

OBJS += \
./src/lib/DPS/lib/dbcc/2bsm.o \
./src/lib/DPS/lib/dbcc/2c.o \
./src/lib/DPS/lib/dbcc/2csv.o \
./src/lib/DPS/lib/dbcc/2json.o \
./src/lib/DPS/lib/dbcc/2xml.o \
./src/lib/DPS/lib/dbcc/can.o \
./src/lib/DPS/lib/dbcc/getopt.o \
./src/lib/DPS/lib/dbcc/main.o \
./src/lib/DPS/lib/dbcc/mpc.o \
./src/lib/DPS/lib/dbcc/parse.o \
./src/lib/DPS/lib/dbcc/util.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/lib/dbcc/%.o: ../src/lib/DPS/lib/dbcc/%.c src/lib/DPS/lib/dbcc/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 -DDEBUG "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-lib-2f-dbcc

clean-src-2f-lib-2f-DPS-2f-lib-2f-dbcc:
	-$(RM) ./src/lib/DPS/lib/dbcc/2bsm.d ./src/lib/DPS/lib/dbcc/2bsm.o ./src/lib/DPS/lib/dbcc/2c.d ./src/lib/DPS/lib/dbcc/2c.o ./src/lib/DPS/lib/dbcc/2csv.d ./src/lib/DPS/lib/dbcc/2csv.o ./src/lib/DPS/lib/dbcc/2json.d ./src/lib/DPS/lib/dbcc/2json.o ./src/lib/DPS/lib/dbcc/2xml.d ./src/lib/DPS/lib/dbcc/2xml.o ./src/lib/DPS/lib/dbcc/can.d ./src/lib/DPS/lib/dbcc/can.o ./src/lib/DPS/lib/dbcc/getopt.d ./src/lib/DPS/lib/dbcc/getopt.o ./src/lib/DPS/lib/dbcc/main.d ./src/lib/DPS/lib/dbcc/main.o ./src/lib/DPS/lib/dbcc/mpc.d ./src/lib/DPS/lib/dbcc/mpc.o ./src/lib/DPS/lib/dbcc/parse.d ./src/lib/DPS/lib/dbcc/parse.o ./src/lib/DPS/lib/dbcc/util.d ./src/lib/DPS/lib/dbcc/util.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-lib-2f-dbcc

