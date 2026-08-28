#include "FabricaMascotas.hpp"

#include "Dragon.hpp"
#include "Gastly.hpp"
#include "Gato.hpp"
#include "Perro.hpp"

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

} // namespace sin nombre

std::unique_ptr<Mascota> FabricaMascotas::crear(TipoMascota tipo, const std::string& nombre)
{
    switch (tipo)
    {
        case TipoMascota::Gastly: return std::make_unique<Gastly>(nombre);
        case TipoMascota::Perro:  return std::make_unique<Perro>(nombre);
        case TipoMascota::Gato:   return std::make_unique<Gato>(nombre);
        case TipoMascota::Dragon: return std::make_unique<Dragon>(nombre);
    }
    return nullptr;
}

std::unique_ptr<Mascota> FabricaMascotas::crear(const std::string& especie, const std::string& nombre)
{
    const std::string clave = enMinusculas(especie);

    if (clave == "gastly") return crear(TipoMascota::Gastly, nombre);
    if (clave == "perro")  return crear(TipoMascota::Perro,  nombre);
    if (clave == "gato")   return crear(TipoMascota::Gato,   nombre);
    if (clave == "dragon") return crear(TipoMascota::Dragon, nombre);

    return nullptr;
}

std::vector<TipoMascota> FabricaMascotas::tiposDisponibles()
{
    return { TipoMascota::Gastly, TipoMascota::Perro, TipoMascota::Gato, TipoMascota::Dragon };
}

std::string FabricaMascotas::nombreTipo(TipoMascota tipo)
{
    switch (tipo)
    {
        case TipoMascota::Gastly: return "Gastly";
        case TipoMascota::Perro:  return "Perro";
        case TipoMascota::Gato:   return "Gato";
        case TipoMascota::Dragon: return "Dragon";
    }
    return "Desconocido";
}

} // namespace vp
