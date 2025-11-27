
CC = clang
CFLAGS = -Wall -Wextra -Werror -ggdb -D_DEBUG

BIN = bin
OBJ = bin_int
SRC = src
INC = include
SCORE = src/runa_core
ICORE = include/runa_core
SVK = src/runa_vulkan
IVK = include/runa_vulkan
LINK_WL = -lwayland-client -lm
LINK_VK = -lvulkan

LAUNCHER_OBJ = launcher.o core.o xdg-shell.o vulkan.o

launcher: $(LAUNCHER_OBJ)
	@mkdir -p $(BIN)
	$(CC) $(OBJ)/launcher.o $(OBJ)/core.o $(OBJ)/xdg-shell.o $(OBJ)/rvulkan.o\
		-o $(BIN)/$@ $(CFLAGS) -I$(ICORE) $(LINK_WL) $(LINK_VK)

launcher.o: $(SRC)/launcher/main.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(ICORE)

core.o: $(SCORE)/unity.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(ICORE) -I$(IVK) -Iexternal

xdg-shell.o: external/wl/xdg-shell.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iexternal/wl

vulkan.o: $(SVK)/vulkan.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(IVK) -I$(ICORE)

all: launcher

clean:
	rm -f $(OBJ)/* $(BIN)/*

.PHONY: all clean
