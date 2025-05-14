################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/src/common/dps_messages.c 

C_DEPS += \
./src/lib/DPS/src/common/dps_messages.d 

OBJS += \
./src/lib/DPS/src/common/dps_messages.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/src/common/%.o: ../src/lib/DPS/src/common/%.c src/lib/DPS/src/common/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wextra -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-src-2f-common

clean-src-2f-lib-2f-DPS-2f-src-2f-common:
	-$(RM) ./src/lib/DPS/src/common/dps_messages.d ./src/lib/DPS/src/common/dps_messages.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-src-2f-common

