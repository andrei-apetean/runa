
CC = clang
CFLAGS = -Wall -Wextra -Werror -ggdb

BIN = bin
OBJ = bin_int
SRC = src
INC = include
INCLUDE_CORE = -I$(INC)/core
LINK_WL = -lwayland-client -lm

LAUNCHER_OBJ = launcher.o core.o xdg-shell.o

launcher: $(LAUNCHER_OBJ)
	@mkdir -p $(BIN)
	$(CC) $(OBJ)/launcher.o $(OBJ)/core.o $(OBJ)/xdg-shell.o -o $(BIN)/$@ $(CFLAGS) $(INCLUDE_CORE) $(LINK_WL)

launcher.o: $(SRC)/launcher/main.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) $(INCLUDE_CORE)

core.o: $(SRC)/core/unity.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) $(INCLUDE_CORE) -Iexternal

xdg-shell.o: external/wl/xdg-shell.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iexternal/wl

all: launcher

clean:
	rm -f $(OBJ)/* $(BIN)/*

.PHONY: all clean
