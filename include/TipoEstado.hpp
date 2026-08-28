#pragma once

#include <string>
#include <vector>

namespace vp {

/**
 * @brief Estados posibles de la mascota. Corresponde al diagrama de estados.
 *
 * Muerta es un estado terminal: una vez que se entra, no se sale.
 */
enum class TipoEstado
{
    Normal,      ///< Todo en orden, sin necesidades urgentes.
    Feliz,       ///< Felicidad alta y ninguna necesidad critica.
    Hambrienta,  ///< Saciedad por debajo del umbral.
    Cansada,     ///< Energia por debajo del umbral.
    Durmiendo,   ///< Recupera energia; no acepta interacciones.
    Enferma,     ///< Salud baja; solo mejora con medicina.
    Jugando,     ///< Estado temporal con duracion fija.
    Muerta       ///< Estado terminal.
};

/// Nombre legible del estado, para mostrarlo en la interfaz.
std::string nombreEstado(TipoEstado tipo);

/// Lista completa de estados (util para recorrer o depurar).
std::vector<TipoEstado> todosLosEstados();

/// Estado a partir de su nombre (sin distinguir mayusculas).
/// Devuelve false si el nombre no corresponde a ningun estado.
bool estadoDesdeNombre(const std::string& nombre, TipoEstado& destino);

} // namespace vp
