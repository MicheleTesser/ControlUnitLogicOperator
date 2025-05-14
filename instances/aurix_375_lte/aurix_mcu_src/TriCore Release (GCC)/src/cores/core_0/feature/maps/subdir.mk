################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cores/core_0/feature/maps/maps.c 

C_DEPS += \
./src/cores/core_0/feature/maps/maps.d 

OBJS += \
./src/cores/core_0/feature/maps/maps.o 


# Each subdirectory must supply rules for building sources it contributes
src/cores/core_0/feature/maps/%.o: ../src/cores/core_0/feature/maps/%.c src/cores/core_0/feature/maps/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -pedantic -pedantic-errors -Wall -Wextra -Werror -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-cores-2f-core_0-2f-feature-2f-maps

clean-src-2f-cores-2f-core_0-2f-feature-2f-maps:
	-$(RM) ./src/cores/core_0/feature/maps/maps.d ./src/cores/core_0/feature/maps/maps.o

.PHONY: clean-src-2f-cores-2f-core_0-2f-feature-2f-maps

