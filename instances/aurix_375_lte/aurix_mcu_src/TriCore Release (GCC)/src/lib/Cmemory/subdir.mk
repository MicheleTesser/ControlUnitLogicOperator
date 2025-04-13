################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/Cmemory/Cmemory.c 

C_DEPS += \
./src/lib/Cmemory/Cmemory.d 

OBJS += \
./src/lib/Cmemory/Cmemory.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/Cmemory/%.o: ../src/lib/Cmemory/%.c src/lib/Cmemory/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wextra -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-Cmemory

clean-src-2f-lib-2f-Cmemory:
	-$(RM) ./src/lib/Cmemory/Cmemory.d ./src/lib/Cmemory/Cmemory.o

.PHONY: clean-src-2f-lib-2f-Cmemory

