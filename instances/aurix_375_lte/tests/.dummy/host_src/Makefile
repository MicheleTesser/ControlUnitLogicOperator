SRC := $(shell find -L . -type f -name \*c)
FLAGS := -Wall -Wextra -Werror -pedantic 
DEBUG := -O0 -fsanitize=address,undefined
RELEASE := -O2

build_debug: $(SRC)
	gcc $(FLAGS) $(DEBUG) $(SRC) -o main

build_release: $(SRC)
	gcc $(FLAGS) $(RELEASE) $(SRC) -o main

clean:
	rm -rf ./main
