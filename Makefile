# Makefile para compilar el proyecto VirtualPet.
# Asegurate de tener instalado MinGW y SFML en tu sistema.
# Este Makefile esta disenado para ser usado en un entorno de Windows con MSYS2.

# Directorios
SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

# Archivos fuente, objetos y ejecutable
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(OBJ_FILES:.o=.d)
EXE := $(BIN_DIR)/game.exe

# Compilador y flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -g -I$(INCLUDE_DIR) -IC:/msys64/ucrt64/include -MMD -MP
LDFLAGS := -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

# Regla principal
all: $(EXE)

# Se enlaza solo cuando algun objeto cambio. Cada .cpp se compila a su propio
# .o, y solo se vuelve a compilar si cambio el o alguna cabecera que incluye
# (eso lo llevan los .d que genera -MMD, incluidos mas abajo).
$(EXE): $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	-mkdir "$(BIN_DIR)"

$(OBJ_DIR): | $(BIN_DIR)
	-mkdir "$(OBJ_DIR)"

-include $(DEP_FILES)

# El juego busca la carpeta assets con rutas relativas, asi que se ejecuta
# desde la raiz del proyecto y no desde bin.
run: all
	./$(EXE)

# Borrar los compilados es sorprendentemente delicado en Windows. bin/ tambien
# guarda la partida guardada, asi que "clean" solo toca el ejecutable y los
# objetos, nunca la carpeta entera.
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
	-rm -rf $(OBJ_DIR)
	-rmdir /S /Q "$(OBJ_DIR)"
	-rm -f $(BIN_DIR)/*.exe
	-del /Q "$(BIN_DIR)\*.exe"

.PHONY: all run clean
