#pragma once

#include "Genero.hpp"
#include "Mascota.hpp"

#include <memory>
#include <string>
#include <vector>

namespace vp {

enum class TipoMascota
{
    Conejo,
    Castor
};

class FabricaMascotas
{
public:
    static std::unique_ptr<Mascota> crear(TipoMascota tipo, Genero genero,
                                          const std::string& nombre);

    static std::unique_ptr<Mascota> crear(const std::string& especie, Genero genero,
                                          const std::string& nombre);

    static std::vector<TipoMascota> tiposDisponibles();
    static std::string              nombreTipo(TipoMascota tipo);
};

}
