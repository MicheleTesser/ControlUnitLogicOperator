################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cores/core_utility/imu/imu.c 

C_DEPS += \
./src/cores/core_utility/imu/imu.d 

OBJS += \
./src/cores/core_utility/imu/imu.o 


# Each subdirectory must supply rules for building sources it contributes
src/cores/core_utility/imu/%.o: ../src/cores/core_utility/imu/%.c src/cores/core_utility/imu/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 -DDEBUG "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-cores-2f-core_utility-2f-imu

clean-src-2f-cores-2f-core_utility-2f-imu:
	-$(RM) ./src/cores/core_utility/imu/imu.d ./src/cores/core_utility/imu/imu.o

.PHONY: clean-src-2f-cores-2f-core_utility-2f-imu

