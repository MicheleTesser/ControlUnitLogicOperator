################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cores/core_0/core_0.c 

C_DEPS += \
./src/cores/core_0/core_0.d 

OBJS += \
./src/cores/core_0/core_0.o 


# Each subdirectory must supply rules for building sources it contributes
src/cores/core_0/%.o: ../src/cores/core_0/%.c src/cores/core_0/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-cores-2f-core_0

clean-src-2f-cores-2f-core_0:
	-$(RM) ./src/cores/core_0/core_0.d ./src/cores/core_0/core_0.o

.PHONY: clean-src-2f-cores-2f-core_0

