################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC37A/Tricore/Flash/Std/IfxFlash.c 

C_DEPS += \
./Libraries/iLLD/TC37A/Tricore/Flash/Std/IfxFlash.d 

OBJS += \
./Libraries/iLLD/TC37A/Tricore/Flash/Std/IfxFlash.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC37A/Tricore/Flash/Std/%.o: ../Libraries/iLLD/TC37A/Tricore/Flash/Std/%.c Libraries/iLLD/TC37A/Tricore/Flash/Std/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Flash-2f-Std

clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Flash-2f-Std:
	-$(RM) ./Libraries/iLLD/TC37A/Tricore/Flash/Std/IfxFlash.d ./Libraries/iLLD/TC37A/Tricore/Flash/Std/IfxFlash.o

.PHONY: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Flash-2f-Std

