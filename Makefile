CC=clang
CFLAGS= -ggdb -std=gnu99
CLIBS=

BUILD_DIR=./build/

SRC=$(wildcard *.c)
# Превращаем список .c файлов в список .o файлов внутри папки build
OBJS=$(patsubst %.c, $(BUILD_DIR)%.o, $(SRC))

# Первое правило в файле выполняется по умолчанию. Теперь это 'all'
.PHONY: default all gf2 valgrind clean

default: all

# Разделяем компиляцию объектов и линковку финального бинарника
all: $(BUILD_DIR)main

# Линковка исполняемого файла из объектных файлов
$(BUILD_DIR)main: $(OBJS)
	@echo "Linking final binary: $@"
	$(CC) $(CFLAGS) $(OBJS) $(CLIBS) -o $@

# Компиляция каждого .c файла в .o файл внутри build/
$(BUILD_DIR)%.o: %.c
	@echo "Building module: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

gf2: all
	gf2 $(shell pwd)/build/main

# Проверка на утечки памяти через Valgrind
valgrind: all
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(BUILD_DIR)main

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) valgrind-out.txt
