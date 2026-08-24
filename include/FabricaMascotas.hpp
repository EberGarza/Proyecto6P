#pragma once

#include "Genero.hpp"
#include "Mascota.hpp"

#include <memory>
#include <string>
#include <vector>

namespace vp {

/// Especies disponibles en el juego.
enum class TipoMascota
{
    Conejo,
    Castor
};

/**
 * @brief Fabrica de mascotas (patron Factory).
 *
 * Centraliza la creacion de objetos concretos. El resto del programa trabaja
 * con punteros a Mascota y no necesita conocer las clases derivadas.
 * Al agregar una especie nueva solo hay que tocar este archivo.
 *
 * El genero no distingue clases: se pasa al constructor y se guarda en la
 * mascota. Por eso no hay un TipoMascota::ConejoHembra, que duplicaria la
 * jerarquia sin anadir comportamiento.
 */
class FabricaMascotas
{
public:
    static std::unique_ptr<Mascota> crear(TipoMascota tipo, Genero genero,
                                          const std::string& nombre);

    /// Version por nombre de especie, usada al cargar una partida guardada.
    static std::unique_ptr<Mascota> crear(const std::string& especie, Genero genero,
                                          const std::string& nombre);

    static std::vector<TipoMascota> tiposDisponibles();
    static std::string              nombreTipo(TipoMascota tipo);
};

} // namespace vp
