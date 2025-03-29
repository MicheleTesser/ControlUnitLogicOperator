################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/main.c 

C_DEPS += \
./src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/main.d 

OBJS += \
./src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/%.o: ../src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/%.c src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-tests-2f-network-2f-multi_slave_one_master-2f-slave_nodes

clean-src-2f-lib-2f-DPS-2f-tests-2f-network-2f-multi_slave_one_master-2f-slave_nodes:
	-$(RM) ./src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/main.d ./src/lib/DPS/tests/network/multi_slave_one_master/slave_nodes/main.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-tests-2f-network-2f-multi_slave_one_master-2f-slave_nodes

