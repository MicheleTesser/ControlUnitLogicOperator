CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 


control_unit_logic_operator_src_path := $(or $(CONTROL_UNIT_LOGIC_OPERATOR_ROOT), .)
ControlUnitLogicOperator.c := $(control_unit_logic_operator_src_path)/ControlUnitLogicOperator.c

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: ControlUnitLogicOperator.o

ControlUnitLogicOperator.o: $(ControlUnitLogicOperator.c) 
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(DEBUG) $(ControlUnitLogicOperator.c) -c

dps_clean:
ifeq ($(wildcard ControlUnitLogicOperator.o), ControlUnitLogicOperator.o)
	rm ControlUnitLogicOperator.o
endif

clean: dps_clean
