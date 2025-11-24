
CC = clang
CFLAGS = -Wall -Wextra -Werror

BIN = bin
OBJ = bin_int
SRC = src
INC = include
INCLUDE_CORE = -I$(INC)/core

LAUNCHER_OBJ = $(OBJ)/launcher.o $(OBJ)/core.o

launcher: $(LAUNCHER_OBJ)
	@mkdir -p $(BIN)
	$(CC) $(LAUNCHER_OBJ) -o $(BIN)/$@ $(CFLAGS) $(INCLUDE_CORE)

$(OBJ)/launcher.o: $(SRC)/launcher/main.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_CORE)

$(OBJ)/core.o: $(SRC)/core/unity.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_CORE)

all: launcher

clean:
	rm -f $(OBJ)/* $(BIN)/*

.PHONY: all clean
