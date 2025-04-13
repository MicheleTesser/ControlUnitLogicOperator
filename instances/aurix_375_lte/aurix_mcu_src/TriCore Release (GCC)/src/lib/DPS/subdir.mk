################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/DPS/dps_master.c \
../src/lib/DPS/dps_slave.c 

C_DEPS += \
./src/lib/DPS/dps_master.d \
./src/lib/DPS/dps_slave.d 

OBJS += \
./src/lib/DPS/dps_master.o \
./src/lib/DPS/dps_slave.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/DPS/%.o: ../src/lib/DPS/%.c src/lib/DPS/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wextra -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-DPS

clean-src-2f-lib-2f-DPS:
	-$(RM) ./src/lib/DPS/dps_master.d ./src/lib/DPS/dps_master.o ./src/lib/DPS/dps_slave.d ./src/lib/DPS/dps_slave.o

.PHONY: clean-src-2f-lib-2f-DPS

