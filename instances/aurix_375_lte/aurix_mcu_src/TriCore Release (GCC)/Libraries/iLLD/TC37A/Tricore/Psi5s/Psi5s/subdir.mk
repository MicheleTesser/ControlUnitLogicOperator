################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/IfxPsi5s_Psi5s.c 

C_DEPS += \
./Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/IfxPsi5s_Psi5s.d 

OBJS += \
./Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/IfxPsi5s_Psi5s.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/%.o: ../Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/%.c Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Psi5s-2f-Psi5s

clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Psi5s-2f-Psi5s:
	-$(RM) ./Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/IfxPsi5s_Psi5s.d ./Libraries/iLLD/TC37A/Tricore/Psi5s/Psi5s/IfxPsi5s_Psi5s.o

.PHONY: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Psi5s-2f-Psi5s

