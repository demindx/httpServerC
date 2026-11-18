CC=clang
CFLAGS=-ggdb -std=c23 -lm
CLIBS=-Iinclude/

BUILD_DIR=build

SRC=$(wildcard src/*.c)

OBJS=$(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRC))

.PHONY: default all valgrind clean

default: all

all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(OBJS)
	@echo "Linking final binary: $@"
	$(CC) $(CFLAGS) $(OBJS) $(CLIBS) -o $@

$(OBJS): $(BUILD_DIR)/%.o: src/%.c
	@echo "Building module: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CLIBS) -c $< -o $@

valgrind: all
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./$(BUILD_DIR)/main

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) valgrind-out.txt
