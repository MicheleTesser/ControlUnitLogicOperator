################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Configurations/Debug/sync_on_halt.c 

C_DEPS += \
./Configurations/Debug/sync_on_halt.d 

OBJS += \
./Configurations/Debug/sync_on_halt.o 


# Each subdirectory must supply rules for building sources it contributes
Configurations/Debug/%.o: ../Configurations/Debug/%.c Configurations/Debug/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Configurations-2f-Debug

clean-Configurations-2f-Debug:
	-$(RM) ./Configurations/Debug/sync_on_halt.d ./Configurations/Debug/sync_on_halt.o

.PHONY: clean-Configurations-2f-Debug

