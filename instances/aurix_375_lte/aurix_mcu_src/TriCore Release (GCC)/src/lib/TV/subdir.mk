################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/TV/Torque_Vectoring_2024_forsecontiu.c \
../src/lib/TV/Torque_Vectoring_2024_forsecontiu_data.c 

C_DEPS += \
./src/lib/TV/Torque_Vectoring_2024_forsecontiu.d \
./src/lib/TV/Torque_Vectoring_2024_forsecontiu_data.d 

OBJS += \
./src/lib/TV/Torque_Vectoring_2024_forsecontiu.o \
./src/lib/TV/Torque_Vectoring_2024_forsecontiu_data.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/TV/%.o: ../src/lib/TV/%.c src/lib/TV/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wextra -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-TV

clean-src-2f-lib-2f-TV:
	-$(RM) ./src/lib/TV/Torque_Vectoring_2024_forsecontiu.d ./src/lib/TV/Torque_Vectoring_2024_forsecontiu.o ./src/lib/TV/Torque_Vectoring_2024_forsecontiu_data.d ./src/lib/TV/Torque_Vectoring_2024_forsecontiu_data.o

.PHONY: clean-src-2f-lib-2f-TV

