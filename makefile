
CC = clang
CFLAGS = -Wall -Wextra -Werror -ggdb -D_DEBUG

BIN = bin
OBJ = bin_int
SRC = src
INC = include
INCLUDE_CORE = -I$(INC)/core
INCLUDE_RVK = -I$(INC)/vulkan
LINK_WL = -lwayland-client -lm
LINK_VK = -lvulkan

LAUNCHER_OBJ = launcher.o core.o xdg-shell.o rvulkan.o

launcher: $(LAUNCHER_OBJ)
	@mkdir -p $(BIN)
	$(CC) $(OBJ)/launcher.o $(OBJ)/core.o $(OBJ)/xdg-shell.o $(OBJ)/rvulkan.o\
		-o $(BIN)/$@ $(CFLAGS) $(INCLUDE_CORE) $(LINK_WL) $(LINK_VK)

launcher.o: $(SRC)/launcher/main.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) $(INCLUDE_CORE)

core.o: $(SRC)/core/unity.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) $(INCLUDE_CORE) $(INCLUDE_RVK) -Iexternal

xdg-shell.o: external/wl/xdg-shell.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iexternal/wl

rvulkan.o: $(SRC)/vulkan/rvulkan.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) $(INCLUDE_RVK) $(INCLUDE_CORE)

all: launcher

clean:
	rm -f $(OBJ)/* $(BIN)/*

.PHONY: all clean
