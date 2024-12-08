################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../src/timers/timers.c" 

COMPILED_SRCS += \
"src/timers/timers.src" 

C_DEPS += \
"./src/timers/timers.d" 

OBJS += \
"src/timers/timers.o" 


# Each subdirectory must supply rules for building sources it contributes
"src/timers/timers.src":"../src/timers/timers.c" "src/timers/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc37x "-fZ:/home/mr_monopoly/Alberto/programmazione/Cproject/raceup/ControlUnitLogicOperator/instances/aurix_375_lte/TriCore Release (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"src/timers/timers.o":"src/timers/timers.src" "src/timers/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-src-2f-timers

clean-src-2f-timers:
	-$(RM) ./src/timers/timers.d ./src/timers/timers.o ./src/timers/timers.src

.PHONY: clean-src-2f-timers

