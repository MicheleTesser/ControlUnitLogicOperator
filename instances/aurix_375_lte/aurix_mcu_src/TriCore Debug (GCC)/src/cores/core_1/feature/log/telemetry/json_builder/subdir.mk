################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cores/core_1/feature/log/telemetry/json_builder/json_builder.c 

C_DEPS += \
./src/cores/core_1/feature/log/telemetry/json_builder/json_builder.d 

OBJS += \
./src/cores/core_1/feature/log/telemetry/json_builder/json_builder.o 


# Each subdirectory must supply rules for building sources it contributes
src/cores/core_1/feature/log/telemetry/json_builder/%.o: ../src/cores/core_1/feature/log/telemetry/json_builder/%.c src/cores/core_1/feature/log/telemetry/json_builder/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-cores-2f-core_1-2f-feature-2f-log-2f-telemetry-2f-json_builder

clean-src-2f-cores-2f-core_1-2f-feature-2f-log-2f-telemetry-2f-json_builder:
	-$(RM) ./src/cores/core_1/feature/log/telemetry/json_builder/json_builder.d ./src/cores/core_1/feature/log/telemetry/json_builder/json_builder.o

.PHONY: clean-src-2f-cores-2f-core_1-2f-feature-2f-log-2f-telemetry-2f-json_builder

