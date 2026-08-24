#pragma once

#include <string>

namespace vp {

/**
 * @brief Sexo de la mascota.
 *
 * No cambia como se comporta la mascota: cambia como se ve (cada especie trae
 * dos hojas de sprites) y como se habla de ella en la bitacora. Se guarda en
 * la partida junto con la especie, porque sin el no se podria volver a cargar
 * la hoja correcta.
 */
enum class Genero
{
    Macho,
    Hembra
};

/// "Macho" / "Hembra", para mostrar y para guardar en la partida.
std::string nombreGenero(Genero genero);

/// "macho" / "hembra": la parte del nombre de archivo de la hoja de sprites.
std::string sufijoGenero(Genero genero);

/// Lee "Macho"/"macho"/"M". Devuelve false si el texto no se reconoce.
bool generoDesdeNombre(const std::string& texto, Genero& destino);

} // namespace vp
