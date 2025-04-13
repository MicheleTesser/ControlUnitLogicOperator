################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/board_dbc/dbc/out_lib/can2/can2.c 

C_DEPS += \
./src/lib/board_dbc/dbc/out_lib/can2/can2.d 

OBJS += \
./src/lib/board_dbc/dbc/out_lib/can2/can2.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/board_dbc/dbc/out_lib/can2/%.o: ../src/lib/board_dbc/dbc/out_lib/can2/%.c src/lib/board_dbc/dbc/out_lib/can2/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 -DDEBUG "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-board_dbc-2f-dbc-2f-out_lib-2f-can2

clean-src-2f-lib-2f-board_dbc-2f-dbc-2f-out_lib-2f-can2:
	-$(RM) ./src/lib/board_dbc/dbc/out_lib/can2/can2.d ./src/lib/board_dbc/dbc/out_lib/can2/can2.o

.PHONY: clean-src-2f-lib-2f-board_dbc-2f-dbc-2f-out_lib-2f-can2

