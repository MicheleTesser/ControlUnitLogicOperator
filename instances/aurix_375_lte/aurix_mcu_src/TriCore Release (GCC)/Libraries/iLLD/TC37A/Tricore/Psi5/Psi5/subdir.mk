################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/IfxPsi5_Psi5.c 

C_DEPS += \
./Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/IfxPsi5_Psi5.d 

OBJS += \
./Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/IfxPsi5_Psi5.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/%.o: ../Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/%.c Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Psi5-2f-Psi5

clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Psi5-2f-Psi5:
	-$(RM) ./Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/IfxPsi5_Psi5.d ./Libraries/iLLD/TC37A/Tricore/Psi5/Psi5/IfxPsi5_Psi5.o

.PHONY: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Psi5-2f-Psi5

