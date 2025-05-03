################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/src/master/dps_master.c 

C_DEPS += \
./src/lib/DPS/src/master/dps_master.d 

OBJS += \
./src/lib/DPS/src/master/dps_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/src/master/%.o: ../src/lib/DPS/src/master/%.c src/lib/DPS/src/master/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 -DDEBUG "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS-2f-src-2f-master

clean-src-2f-lib-2f-DPS-2f-src-2f-master:
	-$(RM) ./src/lib/DPS/src/master/dps_master.d ./src/lib/DPS/src/master/dps_master.o

.PHONY: clean-src-2f-lib-2f-DPS-2f-src-2f-master

