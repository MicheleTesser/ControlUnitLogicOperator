CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

serial_src_path := $(or $(SERIAL_ROOT), .)
serial.c := $(serial_src_path)/serial.c

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: serial.o

serial.o: $(serial.c)
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(serial.c) -c

serial_clean:
ifeq ($(wildcard serial.o), serial.o)
	rm serial.o
endif


clean: serial_clean
