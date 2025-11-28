
CC = clang
CFLAGS = -Wall -Wextra -Werror -ggdb -D_DEBUG

BIN = bin
OBJ = bin_int
SRC = src
INC = include
SCORE = src/runa_core
ICORE = include/runa_core
SRT = src/runa_rt
IRT = include/runa_rt
SVK = src/runa_vulkan
IVK = include/runa_vulkan
LINK_WL = -lwayland-client -lm
LINK_VK = -lvulkan

LAUNCHER_OBJ = launcher.o core.o xdg-shell.o vulkan.o rt.o

launcher: $(LAUNCHER_OBJ)
	@mkdir -p $(BIN)
	$(CC) $(OBJ)/launcher.o $(OBJ)/core.o $(OBJ)/xdg-shell.o $(OBJ)/rt.o -o $(BIN)/$@ $(CFLAGS)  $(LINK_WL)
	
launcher.o: $(SRC)/launcher/launcher_linux.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(ICORE) -Iexternal 

core.o: $(SCORE)/unity.c
	@mkdir -p $(OBJ)
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(ICORE) -I$(IVK)

xdg-shell.o: external/wl/xdg-shell.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -Iexternal

vulkan.o: $(SVK)/vulkan.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(IVK) -I$(ICORE)

rt.o: $(SRT)/unity.c
	$(CC) -c $< -o $(OBJ)/$@ $(CFLAGS) -I$(ICORE)

all: launcher

clean:
	rm -f $(OBJ)/* $(BIN)/*

.PHONY: all clean
