################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.c" 

COMPILED_SRCS += \
"Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.src" 

C_DEPS += \
"./Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.d" 

OBJS += \
"Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.o" 


# Each subdirectory must supply rules for building sources it contributes
"Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.src":"../Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.c" "Libraries/ControlUnitLogicOperator/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fZ:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/TriCore Release (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.o":"Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.src" "Libraries/ControlUnitLogicOperator/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-Libraries-2f-ControlUnitLogicOperator

clean-Libraries-2f-ControlUnitLogicOperator:
	-$(RM) ./Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.d ./Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.o ./Libraries/ControlUnitLogicOperator/ControlUnitLogicOperator.src

.PHONY: clean-Libraries-2f-ControlUnitLogicOperator

