/**
 * @file main.cpp
 * @brief Punto de entrada de la version grafica (SFML).
 *
 * Toda la logica del juego vive en las clases; main solo crea el objeto Juego
 * y le cede el control.
 *
 */
// Requiere SFML 3 (en MSYS2: pacman -S mingw-w64-ucrt-x86_64-sfml).
// Compilar:
//   g++ -std=c++17 -Iinclude src/nucleo/*.cpp src/grafico/*.cpp src/main.cpp
//       -lsfml-graphics -lsfml-window -lsfml-system -o build/VirtualPet.exe
//
// Ejecutar desde la raiz del proyecto: busca assets/ con rutas relativas.

#include "Juego.hpp"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        vp::Juego juego;
        return juego.ejecutar();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error fatal: " << error.what() << "\n";
        return 1;
    }
}
