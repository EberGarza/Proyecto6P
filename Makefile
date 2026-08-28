# Makefile para compilar el proyecto VirtualPet.
# Asegurate de tener instalado MinGW y SFML en tu sistema.
# Este Makefile esta disenado para ser usado en un entorno de Windows con MSYS2.

# Directorios
SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin

# Archivos fuente y ejecutable
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
EXE := $(BIN_DIR)/game.exe

# Compilador y flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I$(INCLUDE_DIR) -IC:/msys64/ucrt64/include -g
LDFLAGS := -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system

# Regla principal
all: $(EXE)

$(EXE): $(SRC_FILES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR):
	-mkdir "$(BIN_DIR)"

# El juego busca la carpeta assets con rutas relativas, asi que se ejecuta
# desde la raiz del proyecto y no desde bin.
run: all
	./$(EXE)

clean:
	-rm -f $(BIN_DIR)/*.exe

.PHONY: all run clean
