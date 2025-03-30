################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver_imp/can/can.c 

C_DEPS += \
./driver_imp/can/can.d 

OBJS += \
./driver_imp/can/can.o 


# Each subdirectory must supply rules for building sources it contributes
driver_imp/can/%.o: ../driver_imp/can/%.c driver_imp/can/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-driver_imp-2f-can

clean-driver_imp-2f-can:
	-$(RM) ./driver_imp/can/can.d ./driver_imp/can/can.o

.PHONY: clean-driver_imp-2f-can

