CC = clang
CFLAGS = -Wall -g
BREW_PREFIX = $(shell brew --prefix)
INCLUDE_DIRS = -Iinclude -I$(BREW_PREFIX)/include
LIB_DIRS = -L$(BREW_PREFIX)/lib
LIBS = -lcurl -ljansson
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, bin/%.o, $(SRCS))
TARGET = bin/stockAnalyzer
all: $(TARGET)
$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)
	@echo "Build complete. Executable is at $(TARGET)"
bin/%.o: src/%.c
	@mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@
clean:
	rm -rf bin