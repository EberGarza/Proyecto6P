#pragma once

#include "Mascota.hpp"

#include <memory>
#include <string>
#include <vector>

namespace vp {

/// Especies disponibles en el juego.
enum class TipoMascota
{
    Gastly,
    Perro,
    Gato,
    Dragon
};

/**
 * @brief Fabrica de mascotas (patron Factory).
 *
 * Centraliza la creacion de objetos concretos. El resto del programa trabaja
 * con punteros a Mascota y no necesita conocer las clases derivadas.
 * Al agregar una especie nueva solo hay que tocar este archivo.
 */
class FabricaMascotas
{
public:
    static std::unique_ptr<Mascota> crear(TipoMascota tipo, const std::string& nombre);

    /// Version por nombre de especie, usada al cargar una partida guardada.
    static std::unique_ptr<Mascota> crear(const std::string& especie, const std::string& nombre);

    static std::vector<TipoMascota> tiposDisponibles();
    static std::string              nombreTipo(TipoMascota tipo);
};

} // namespace vp
