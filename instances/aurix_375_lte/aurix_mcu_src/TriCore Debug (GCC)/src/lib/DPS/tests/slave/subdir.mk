################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/tests/slave/main.c 

C_DEPS += \
./src/lib/DPS/tests/slave/main.d 

OBJS += \
./src/lib/DPS/tests/slave/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/tests/slave/%.o: ../src/lib/DPS/tests/slave/%.c src/lib/DPS/tests/slave/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-tests-2f-slave

clean-src-2f-lib-2f-DPS-2f-tests-2f-slave:
	-$(RM) ./src/lib/DPS/tests/slave/main.d ./src/lib/DPS/tests/slave/main.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-tests-2f-slave

