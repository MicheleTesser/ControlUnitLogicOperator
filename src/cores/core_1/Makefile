CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

core1_src_path := $(or $(CORE_1_ROOT), .)
core1.c := $(core1_src_path)/core1.c

all: release

DEBUG_FLAGS = -O1 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: core1.o

core1.o: $(core1.c) 
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(DEBUG) $(core1.c) -c

core_1_clean:
ifeq ($(wildcard core1.o), core1.o)
	rm core1.o
endif

clean: core_1_clean
