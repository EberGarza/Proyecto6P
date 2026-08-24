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
LDFLAGS := -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

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

# Borrar el ejecutable es sorprendentemente delicado en Windows.
#
# make elige con que ejecuta cada receta segun tenga sh a mano o no:
#
#   - Desde la consola MSYS2 encuentra sh, asi que funciona "rm" y falla "del".
#   - Desde PowerShell no lo encuentra y tira de cmd, asi que es al reves.
#
# Se ponen las dos, cada una con "-" delante para que su fallo no detenga a
# make. Una de las dos suelta un "command not found" por consola: es normal.
#
# Y un aviso: NO poner "2>NUL" para callar ese mensaje. Bajo sh eso no descarta
# nada, crea un archivo llamado NUL en la carpeta, y git no puede indexarlo.
clean:
	-rm -f $(BIN_DIR)/*.exe
	-del /Q "$(BIN_DIR)\*.exe"

.PHONY: all run clean
