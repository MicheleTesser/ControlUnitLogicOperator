################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/board_dbc/dbc/dbcc/2bsm.c \
../src/lib/board_dbc/dbc/dbcc/2c.c \
../src/lib/board_dbc/dbc/dbcc/2csv.c \
../src/lib/board_dbc/dbc/dbcc/2json.c \
../src/lib/board_dbc/dbc/dbcc/2xml.c \
../src/lib/board_dbc/dbc/dbcc/can.c \
../src/lib/board_dbc/dbc/dbcc/getopt.c \
../src/lib/board_dbc/dbc/dbcc/main.c \
../src/lib/board_dbc/dbc/dbcc/mpc.c \
../src/lib/board_dbc/dbc/dbcc/parse.c \
../src/lib/board_dbc/dbc/dbcc/util.c 

O_SRCS += \
../src/lib/board_dbc/dbc/dbcc/2bsm.o \
../src/lib/board_dbc/dbc/dbcc/2c.o \
../src/lib/board_dbc/dbc/dbcc/2csv.o \
../src/lib/board_dbc/dbc/dbcc/2json.o \
../src/lib/board_dbc/dbc/dbcc/2xml.o \
../src/lib/board_dbc/dbc/dbcc/can.o \
../src/lib/board_dbc/dbc/dbcc/getopt.o \
../src/lib/board_dbc/dbc/dbcc/main.o \
../src/lib/board_dbc/dbc/dbcc/mpc.o \
../src/lib/board_dbc/dbc/dbcc/parse.o \
../src/lib/board_dbc/dbc/dbcc/util.o 

C_DEPS += \
./src/lib/board_dbc/dbc/dbcc/2bsm.d \
./src/lib/board_dbc/dbc/dbcc/2c.d \
./src/lib/board_dbc/dbc/dbcc/2csv.d \
./src/lib/board_dbc/dbc/dbcc/2json.d \
./src/lib/board_dbc/dbc/dbcc/2xml.d \
./src/lib/board_dbc/dbc/dbcc/can.d \
./src/lib/board_dbc/dbc/dbcc/getopt.d \
./src/lib/board_dbc/dbc/dbcc/main.d \
./src/lib/board_dbc/dbc/dbcc/mpc.d \
./src/lib/board_dbc/dbc/dbcc/parse.d \
./src/lib/board_dbc/dbc/dbcc/util.d 

OBJS += \
./src/lib/board_dbc/dbc/dbcc/2bsm.o \
./src/lib/board_dbc/dbc/dbcc/2c.o \
./src/lib/board_dbc/dbc/dbcc/2csv.o \
./src/lib/board_dbc/dbc/dbcc/2json.o \
./src/lib/board_dbc/dbc/dbcc/2xml.o \
./src/lib/board_dbc/dbc/dbcc/can.o \
./src/lib/board_dbc/dbc/dbcc/getopt.o \
./src/lib/board_dbc/dbc/dbcc/main.o \
./src/lib/board_dbc/dbc/dbcc/mpc.o \
./src/lib/board_dbc/dbc/dbcc/parse.o \
./src/lib/board_dbc/dbc/dbcc/util.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/board_dbc/dbc/dbcc/%.o: ../src/lib/board_dbc/dbc/dbcc/%.c src/lib/board_dbc/dbc/dbcc/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-board_dbc-2f-dbc-2f-dbcc

clean-src-2f-lib-2f-board_dbc-2f-dbc-2f-dbcc:
	-$(RM) ./src/lib/board_dbc/dbc/dbcc/2bsm.d ./src/lib/board_dbc/dbc/dbcc/2bsm.o ./src/lib/board_dbc/dbc/dbcc/2c.d ./src/lib/board_dbc/dbc/dbcc/2c.o ./src/lib/board_dbc/dbc/dbcc/2csv.d ./src/lib/board_dbc/dbc/dbcc/2csv.o ./src/lib/board_dbc/dbc/dbcc/2json.d ./src/lib/board_dbc/dbc/dbcc/2json.o ./src/lib/board_dbc/dbc/dbcc/2xml.d ./src/lib/board_dbc/dbc/dbcc/2xml.o ./src/lib/board_dbc/dbc/dbcc/can.d ./src/lib/board_dbc/dbc/dbcc/can.o ./src/lib/board_dbc/dbc/dbcc/getopt.d ./src/lib/board_dbc/dbc/dbcc/getopt.o ./src/lib/board_dbc/dbc/dbcc/main.d ./src/lib/board_dbc/dbc/dbcc/main.o ./src/lib/board_dbc/dbc/dbcc/mpc.d ./src/lib/board_dbc/dbc/dbcc/mpc.o ./src/lib/board_dbc/dbc/dbcc/parse.d ./src/lib/board_dbc/dbc/dbcc/parse.o ./src/lib/board_dbc/dbc/dbcc/util.d ./src/lib/board_dbc/dbc/dbcc/util.o

.PHONY: clean-src-2f-lib-2f-board_dbc-2f-dbc-2f-dbcc

