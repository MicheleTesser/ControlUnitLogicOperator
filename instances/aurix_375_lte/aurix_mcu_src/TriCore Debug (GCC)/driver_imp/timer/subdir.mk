################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver_imp/timer/timer.c 

C_DEPS += \
./driver_imp/timer/timer.d 

OBJS += \
./driver_imp/timer/timer.o 


# Each subdirectory must supply rules for building sources it contributes
driver_imp/timer/%.o: ../driver_imp/timer/%.c driver_imp/timer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 -DDEBUG "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -pedantic -pedantic-errors -Wall -Wextra -Werror -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-driver_imp-2f-timer

clean-driver_imp-2f-timer:
	-$(RM) ./driver_imp/timer/timer.d ./driver_imp/timer/timer.o

.PHONY: clean-driver_imp-2f-timer

