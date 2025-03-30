################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver_imp/ethernet/ethernet.c 

C_DEPS += \
./driver_imp/ethernet/ethernet.d 

OBJS += \
./driver_imp/ethernet/ethernet.o 


# Each subdirectory must supply rules for building sources it contributes
driver_imp/ethernet/%.o: ../driver_imp/ethernet/%.c driver_imp/ethernet/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-driver_imp-2f-ethernet

clean-driver_imp-2f-ethernet:
	-$(RM) ./driver_imp/ethernet/ethernet.d ./driver_imp/ethernet/ethernet.o

.PHONY: clean-driver_imp-2f-ethernet

