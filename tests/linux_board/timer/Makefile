CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

timer_src_path := $(or $(TIMER_ROOT), .)
timer.c := $(timer_src_path)/timer.c

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: timer.o

timer.o: $(timer.c)
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(timer.c) -c

timer_clean:
ifeq ($(wildcard timer.o), timer.o)
	rm timer.o
endif


clean: timer_clean
