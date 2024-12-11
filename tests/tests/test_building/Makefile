CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS += 

MAIN_ROOT := .

SCORE_LIB_ROOT := $(MAIN_ROOT)/score_lib
include $(SCORE_LIB_ROOT)/Makefile

CONTROL_UNIT_LOGIC_OPERATOR_ROOT := $(MAIN_ROOT)/src
include $(CONTROL_UNIT_LOGIC_OPERATOR_ROOT)/Makefile

TIMER_ROOT = $(MAIN_ROOT)/linux_board/timer
include  $(TIMER_ROOT)/Makefile

GPIO_ROOT = $(MAIN_ROOT)/linux_board/gpio
include  $(GPIO_ROOT)/Makefile

CAN_ROOT = $(MAIN_ROOT)/linux_board/can
include  $(CAN_ROOT)/Makefile

SERIAL_ROOT = $(MAIN_ROOT)/linux_board/serial
include  $(SERIAL_ROOT)/Makefile

OBJ_FILE = $(wildcard *.o)

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile

compile: main

main: main.c
	gcc $(C_FLAGS) main.c $(OBJ_FILE) $(C_EXFLAGS) -o main

clean:
ifeq ($(wildcard main),main)
	rm main
endif

