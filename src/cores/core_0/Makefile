CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS += 

core0_src_path := $(or $(CORE_0_ROOT), .)
core0.c := $(core0_src_path)/core0.c

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: core0.o

core0.o: $(core0.c) 
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(DEBUG) $(core0.c) -c

core_0_clean:
ifeq ($(wildcard core0.o), core0.o)
	rm core0.o
endif

clean: core_0_clean
