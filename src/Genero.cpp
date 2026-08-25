#include "Genero.hpp"

#include <algorithm>
#include <cctype>

namespace vp {
namespace {

std::string enMinusculas(std::string texto)
{
    std::transform(texto.begin(), texto.end(), texto.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return texto;
}

}

std::string nombreGenero(Genero genero)
{
    return (genero == Genero::Macho) ? "Macho" : "Hembra";
}

std::string sufijoGenero(Genero genero)
{
    return (genero == Genero::Macho) ? "macho" : "hembra";
}

bool generoDesdeNombre(const std::string& texto, Genero& destino)
{
    const std::string clave = enMinusculas(texto);

    if (clave == "macho"  || clave == "m") { destino = Genero::Macho;  return true; }
    if (clave == "hembra" || clave == "h") { destino = Genero::Hembra; return true; }

    return false;
}

}
