################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/tests/network/can_lib/canlib.c \
../src/lib/DPS/tests/network/can_lib/main.c 

C_DEPS += \
./src/lib/DPS/tests/network/can_lib/canlib.d \
./src/lib/DPS/tests/network/can_lib/main.d 

OBJS += \
./src/lib/DPS/tests/network/can_lib/canlib.o \
./src/lib/DPS/tests/network/can_lib/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/tests/network/can_lib/%.o: ../src/lib/DPS/tests/network/can_lib/%.c src/lib/DPS/tests/network/can_lib/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-tests-2f-network-2f-can_lib

clean-src-2f-lib-2f-DPS-2f-tests-2f-network-2f-can_lib:
	-$(RM) ./src/lib/DPS/tests/network/can_lib/canlib.d ./src/lib/DPS/tests/network/can_lib/canlib.o ./src/lib/DPS/tests/network/can_lib/main.d ./src/lib/DPS/tests/network/can_lib/main.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-tests-2f-network-2f-can_lib

