################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22.c \
../src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22_data.c \
../src/lib/rigen_fun_simulink22_ert_rtw/rtGetInf.c \
../src/lib/rigen_fun_simulink22_ert_rtw/rtGetNaN.c \
../src/lib/rigen_fun_simulink22_ert_rtw/rt_nonfinite.c 

C_DEPS += \
./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22.d \
./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22_data.d \
./src/lib/rigen_fun_simulink22_ert_rtw/rtGetInf.d \
./src/lib/rigen_fun_simulink22_ert_rtw/rtGetNaN.d \
./src/lib/rigen_fun_simulink22_ert_rtw/rt_nonfinite.d 

OBJS += \
./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22.o \
./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22_data.o \
./src/lib/rigen_fun_simulink22_ert_rtw/rtGetInf.o \
./src/lib/rigen_fun_simulink22_ert_rtw/rtGetNaN.o \
./src/lib/rigen_fun_simulink22_ert_rtw/rt_nonfinite.o 


# Each subdirectory must supply rules for building sources it contributes
src/lib/rigen_fun_simulink22_ert_rtw/%.o: ../src/lib/rigen_fun_simulink22_ert_rtw/%.c src/lib/rigen_fun_simulink22_ert_rtw/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c11 "@Z:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/aurix_mcu_src/TriCore Debug (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -Og -g3 -gdwarf-3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc162 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-lib-2f-rigen_fun_simulink22_ert_rtw

clean-src-2f-lib-2f-rigen_fun_simulink22_ert_rtw:
	-$(RM) ./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22.d ./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22.o ./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22_data.d ./src/lib/rigen_fun_simulink22_ert_rtw/rigen_fun_simulink22_data.o ./src/lib/rigen_fun_simulink22_ert_rtw/rtGetInf.d ./src/lib/rigen_fun_simulink22_ert_rtw/rtGetInf.o ./src/lib/rigen_fun_simulink22_ert_rtw/rtGetNaN.d ./src/lib/rigen_fun_simulink22_ert_rtw/rtGetNaN.o ./src/lib/rigen_fun_simulink22_ert_rtw/rt_nonfinite.d ./src/lib/rigen_fun_simulink22_ert_rtw/rt_nonfinite.o

.PHONY: clean-src-2f-lib-2f-rigen_fun_simulink22_ert_rtw

