CC=gcc
CFLAGS= -g -std=c23
CLIBS=

BUILD_DIR=./build/

SRC=$(wildcard *.c)
OBJS=$(patsubst %.c, $(BUILD_DIR)%.o, $(SRC))

.PHONY: all valgrind clean

main: main.c
	$(GCC) $(CFLAGS) main.c -o main

clean:
	rm -rf $(BUILD_DIR)

all:$(OBJS)
	@echo "Building main module"
	$(CC) $(CFLAGS) $(OBJS) $(CLIBS) -o $(BUILD_DIR)main
	$(BUILD_DIR)main

$(BUILD_DIR)%.o:%.c
	@echo "Building $@ module"
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

valgrind:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(BUILD_DIR)main
