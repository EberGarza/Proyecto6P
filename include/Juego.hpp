#pragma once

#include "GestorRecursos.hpp"
#include "Pantalla.hpp"

#include <SFML/Graphics.hpp>

#include <memory>

namespace vp {

/**
 * @brief Clase principal de la aplicacion: administra la ventana y el bucle.
 *
 * Estructura clasica de un juego:
 *   ejecutar() -> mientras la ventana este abierta:
 *                   procesarEventos() -> actualizar(dt) -> dibujar()
 *
 * Juego no conoce las pantallas concretas: trabaja con la interfaz Pantalla y
 * solo cambia de una a otra cuando la pantalla activa se lo pide.
 */
class Juego
{
public:
    Juego();
    ~Juego();

    Juego(const Juego&)            = delete;
    Juego& operator=(const Juego&) = delete;

    /// Arranca el bucle principal. Devuelve el codigo de salida del programa.
    int ejecutar();

private:
    bool inicializar();

    /// Carga la fuente de la interfaz, con varias rutas de respaldo.
    bool cargarFuente();

    void cambiarPantalla(Pantalla::Transicion destino);
    void procesarEventos();
    void actualizar(float dt);
    void dibujar();
    void alCerrar();

    static constexpr unsigned kAnchoVentana = 1024;
    static constexpr unsigned kAltoVentana  = 640;
    static constexpr float    kDtMaximo     = 0.1f;  ///< evita saltos tras un tiron

    sf::RenderWindow            ventana_;
    GestorRecursos<sf::Font>    fuentes_;
    std::unique_ptr<Pantalla>   pantalla_;
    bool                        listo_ = false;
};

} // namespace vp
