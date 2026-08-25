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
