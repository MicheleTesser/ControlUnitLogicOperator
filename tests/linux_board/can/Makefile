CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

can_src_path := $(or $(CAN_ROOT), .)
can.c := $(can_src_path)/can.c

ifndef $(CANLIB_ROOT)
CANLIB_ROOT := $(can_src_path)/can_lib
include $(CANLIB_ROOT)/Makefile
endif

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: can.o

can.o: $(can.c)
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(can.c) -c

can_clean:
ifeq ($(wildcard can.o), can.o)
	rm can.o
endif


clean: can_clean
