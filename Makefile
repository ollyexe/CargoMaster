CC = gcc
CFLAGS = -std=c89 -Wpedantic -Isrc/include -g -D_GNU_SOURCE

# Library flags
LDFLAGS = -Lsrc/lib

# Source directories
SRC_DIR = src
LIB_DIR = $(SRC_DIR)/lib
INCLUDE_DIR = $(SRC_DIR)/include
BIN_DIR = bin

# List of source files
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
MAIN_SRC = $(SRC_DIR)/main.c

# List of object files
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c,$(BIN_DIR)/%.o,$(LIB_SRCS))
MAIN_OBJ = $(BIN_DIR)/main.o

# Name of the final executable
TARGET = sea
all: clean build run

build: $(TARGET)

$(TARGET): $(LIB_OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$@ $^

$(BIN_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(BIN_DIR)/$(TARGET)


clean:
	rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/$(TARGET)