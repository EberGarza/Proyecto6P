#pragma once

#include <string>

namespace vp {

enum class Genero
{
    Macho,
    Hembra
};

std::string nombreGenero(Genero genero);

std::string sufijoGenero(Genero genero);

bool generoDesdeNombre(const std::string& texto, Genero& destino);

}
