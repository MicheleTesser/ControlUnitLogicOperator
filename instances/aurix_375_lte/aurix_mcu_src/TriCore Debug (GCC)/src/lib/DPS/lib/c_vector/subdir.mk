################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/lib/c_vector/c_vector.c 

C_DEPS += \
./src/lib/DPS/lib/c_vector/c_vector.d 

OBJS += \
./src/lib/DPS/lib/c_vector/c_vector.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/lib/c_vector/%.o: ../src/lib/DPS/lib/c_vector/%.c src/lib/DPS/lib/c_vector/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-lib-2f-c_vector

clean-src-2f-lib-2f-DPS-2f-lib-2f-c_vector:
	-$(RM) ./src/lib/DPS/lib/c_vector/c_vector.d ./src/lib/DPS/lib/c_vector/c_vector.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-lib-2f-c_vector

