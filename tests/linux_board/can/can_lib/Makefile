CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS += 

canlib_src_path := $(or $(CANLIB_ROOT), .)
canlib.c := $(canlib_src_path)/canlib.c

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile

compile: canlib.o

canlib.o: $(canlib.c)
	gcc $(C_FLAGS) $(canlib.c) -c

canlib_clean:
ifeq ($(wildcard canlib.o),canlib.o)
	rm canlib.o
endif

clean: canlib_clean
