#pragma once

#include <memory>
#include <string>

namespace vp {

class Mascota;

/**
 * @brief Guarda y recupera la partida en un archivo de texto.
 *
 * Formato: una linea por dato, con la forma clave=valor. Es sencillo de leer
 * a mano, lo que ayuda al depurar y al revisar el proyecto.
 */
class GestorGuardado
{
public:
    /// Escribe el estado de la mascota. Devuelve false si no pudo abrir el archivo.
    static bool guardar(const Mascota& mascota, const std::string& ruta);

    /// Reconstruye la mascota. Devuelve nullptr si el archivo falta o esta corrupto.
    static std::unique_ptr<Mascota> cargar(const std::string& ruta);

    static bool existePartida(const std::string& ruta);

    /// Ruta usada cuando el jugador no indica otra.
    static std::string rutaPorDefecto();

private:
    static constexpr int kVersionFormato = 1;
};

} // namespace vp
