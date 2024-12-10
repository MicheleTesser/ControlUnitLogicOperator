CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

core2_src_path := $(or $(CORE_2_ROOT), .)
core2.c := $(core2_src_path)/core2.c

all: release

DEBUG_FLAGS = -O2 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: core2.o

core2.o: $(core2.c) 
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(DEBUG) $(core2.c) -c

core_2_clean:
ifeq ($(wildcard core2.o), core2.o)
	rm core2.o
endif

clean: core_2_clean
