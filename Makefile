CC = gcc
CFLAGS = -std=c89 -Wpedantic -Isrc/include -g -D_GNU_SOURCE -lm

# Source directories
SRC_DIR = src
LIB_DIR = $(SRC_DIR)/lib
INCLUDE_DIR = $(SRC_DIR)/include
BIN_DIR = bin
PROCESSI_DIR = $(BIN_DIR)/processi

# List of source files
LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
NAVE_SRC = $(SRC_DIR)/lib/Nave.c
PORTO_SRC = $(SRC_DIR)/lib/Porto.c
UTIL_SRC = $(SRC_DIR)/lib/Util.c
DUMP_SRC = $(SRC_DIR)/lib/Dump.c
MASTER_SRC = $(SRC_DIR)/Master.c

# List of object files
LIB_OBJS = $(patsybst $(LIB_DIR)/%.c,$(BIN_DIR)/%.o,$(LIB_SRCS))
NAVE_OBJ = $(BIN_DIR)/Nave.o
PORTO_OBJ = $(BIN_DIR)/Porto.o
MASTER_OBJ = $(BIN_DIR)/Master.o
UTIL_OBJ = $(BIN_DIR)/Util.o
DUMP_OBJ = $(BIN_DIR)/Dump.o

# Names of the final executables
NAVE_EXEC = $(PROCESSI_DIR)/nave
PORTO_EXEC = $(PROCESSI_DIR)/porto
MASTER_EXEC = $(PROCESSI_DIR)/master

all: $(NAVE_EXEC) $(PORTO_EXEC) $(MASTER_EXEC)


$(BIN_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAVE_OBJ): $(NAVE_SRC) $(UTIL_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(PORTO_OBJ): $(PORTO_SRC) $(UTIL_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(MASTER_OBJ): $(MASTER_SRC) $(UTIL_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAVE_EXEC): $(LIB_OBJS) $(NAVE_OBJ) $(UTIL_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lm

$(PORTO_EXEC): $(LIB_OBJS) $(PORTO_OBJ) $(UTIL_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lm

$(MASTER_EXEC): $(LIB_OBJS) $(MASTER_OBJ) $(UTIL_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lm

run :
	./$(MASTER_EXEC)

clean:
	rm -rf $(BIN_DIR)/*.o $(PROCESSI_DIR)/*
rmIPC:
	ipcrm -m 0

