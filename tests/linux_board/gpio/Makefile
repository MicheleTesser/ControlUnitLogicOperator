CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

gpio_src_path := $(or $(GPIO_ROOT), .)
gpio.c := $(gpio_src_path)/gpio.c

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: gpio.o

gpio.o: $(gpio.c)
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(gpio.c) -c

gpio_clean:
ifeq ($(wildcard gpio.o), gpio.o)
	rm gpio.o
endif


clean: gpio_clean
