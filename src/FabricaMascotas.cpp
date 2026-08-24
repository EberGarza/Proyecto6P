#include "FabricaMascotas.hpp"

#include "Castor.hpp"
#include "Conejo.hpp"

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

std::unique_ptr<Mascota> FabricaMascotas::crear(TipoMascota tipo, Genero genero,
                                                const std::string& nombre)
{
    switch (tipo)
    {
        case TipoMascota::Conejo: return std::make_unique<Conejo>(nombre, genero);
        case TipoMascota::Castor: return std::make_unique<Castor>(nombre, genero);
    }
    return nullptr;
}

std::unique_ptr<Mascota> FabricaMascotas::crear(const std::string& especie, Genero genero,
                                                const std::string& nombre)
{
    const std::string clave = enMinusculas(especie);

    if (clave == "conejo") return crear(TipoMascota::Conejo, genero, nombre);
    if (clave == "castor") return crear(TipoMascota::Castor, genero, nombre);

    return nullptr;
}

std::vector<TipoMascota> FabricaMascotas::tiposDisponibles()
{
    return { TipoMascota::Conejo, TipoMascota::Castor };
}

std::string FabricaMascotas::nombreTipo(TipoMascota tipo)
{
    switch (tipo)
    {
        case TipoMascota::Conejo: return "Conejo";
        case TipoMascota::Castor: return "Castor";
    }
    return "Desconocido";
}

} // namespace vp
