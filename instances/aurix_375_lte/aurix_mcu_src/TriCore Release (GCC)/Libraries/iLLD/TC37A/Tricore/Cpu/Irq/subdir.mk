################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC37A/Tricore/Cpu/Irq/IfxCpu_Irq.c 

C_DEPS += \
./Libraries/iLLD/TC37A/Tricore/Cpu/Irq/IfxCpu_Irq.d 

OBJS += \
./Libraries/iLLD/TC37A/Tricore/Cpu/Irq/IfxCpu_Irq.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC37A/Tricore/Cpu/Irq/%.o: ../Libraries/iLLD/TC37A/Tricore/Cpu/Irq/%.c Libraries/iLLD/TC37A/Tricore/Cpu/Irq/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Cpu-2f-Irq

clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Cpu-2f-Irq:
	-$(RM) ./Libraries/iLLD/TC37A/Tricore/Cpu/Irq/IfxCpu_Irq.d ./Libraries/iLLD/TC37A/Tricore/Cpu/Irq/IfxCpu_Irq.o

.PHONY: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Cpu-2f-Irq

