################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver_imp/serial/serial.c 

C_DEPS += \
./driver_imp/serial/serial.d 

OBJS += \
./driver_imp/serial/serial.o 


# Each subdirectory must supply rules for building sources it contributes
driver_imp/serial/%.o: ../driver_imp/serial/%.c driver_imp/serial/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 -DDEBUG "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -pedantic -pedantic-errors -Wall -Wextra -Werror -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-driver_imp-2f-serial

clean-driver_imp-2f-serial:
	-$(RM) ./driver_imp/serial/serial.d ./driver_imp/serial/serial.o

.PHONY: clean-driver_imp-2f-serial

