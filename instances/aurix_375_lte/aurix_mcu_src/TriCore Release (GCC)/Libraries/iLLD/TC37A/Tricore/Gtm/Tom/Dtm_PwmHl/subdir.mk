################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/IfxGtm_Tom_Dtm_PwmHl.c 

C_DEPS += \
./Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/IfxGtm_Tom_Dtm_PwmHl.d 

OBJS += \
./Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/IfxGtm_Tom_Dtm_PwmHl.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/%.o: ../Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/%.c Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Gtm-2f-Tom-2f-Dtm_PwmHl

clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Gtm-2f-Tom-2f-Dtm_PwmHl:
	-$(RM) ./Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/IfxGtm_Tom_Dtm_PwmHl.d ./Libraries/iLLD/TC37A/Tricore/Gtm/Tom/Dtm_PwmHl/IfxGtm_Tom_Dtm_PwmHl.o

.PHONY: clean-Libraries-2f-iLLD-2f-TC37A-2f-Tricore-2f-Gtm-2f-Tom-2f-Dtm_PwmHl

