
CC = clang
CFLAGS = -Wall -Wextra -Werror -ggdb -D_DEBUG

BIN = bin
OBJ = bin_int
MODS = bin/modules
LINK_WL = -lwayland-client -lm
LINK_VK = -lvulkan

LAUNCHER_OBJ = launcher.o xdg-shell.o rt.o

launcher: $(LAUNCHER_OBJ)
	@mkdir -p $(MODS)
	$(CC) $(OBJ)/launcher.o $(OBJ)/xdg-shell.o $(OBJ)/rt.o -o $(BIN)/$@ $(CFLAGS) $(LINK_WL)

vulkan: runa_vulkan.o
	$(CC) $(OBJ)/$< -o $(BIN)/modules/runa_vulkan.so -shared  $(LINK_VK)

launcher.o: src/launcher/launcher_linux.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iinclude -Iexternal


xdg-shell.o: external/wl/xdg-shell.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iexternal

rt.o: src/runa_rt/unity.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iinclude/runa_core -Iinclude


runa_vulkan.o: src/runa_vulkan/unity.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iinclude/runa_vulkan -Iinclude -fPIC

all: launcher vulkan

clean:
	rm -rf $(OBJ)/* $(BIN)/*

.PHONY: all clean
