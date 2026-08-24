#pragma once

#include <SFML/Graphics.hpp>

namespace vp {

/**
 * @brief Clase base abstracta de las pantallas del juego (patron State a nivel
 *        de aplicacion).
 *
 * La clase Juego solo conoce esta interfaz: procesa eventos, actualiza y dibuja
 * la pantalla activa, y cuando esta pide un cambio, la sustituye. Agregar una
 * pantalla nueva (tienda, creditos, ajustes) no obliga a tocar el bucle.
 */
class Pantalla
{
public:
    /// Que quiere hacer la pantalla cuando termina.
    enum class Transicion
    {
        Ninguna,     ///< seguir en esta pantalla
        Menu,        ///< volver al menu principal
        Opciones,    ///< ir a la pantalla de opciones
        Seleccion,   ///< ir a la pantalla de eleccion de mascota
        Juego,       ///< ir a la pantalla de juego
        Salir        ///< cerrar la aplicacion
    };

    virtual ~Pantalla() = default;

    virtual void manejarEvento(const sf::Event& evento)   = 0;
    virtual void actualizar(float dt)                     = 0;
    virtual void dibujar(sf::RenderTarget& objetivo) const = 0;

    Transicion transicion() const { return transicion_; }
    void       consumirTransicion() { transicion_ = Transicion::Ninguna; }

protected:
    void solicitar(Transicion destino) { transicion_ = destino; }

private:
    Transicion transicion_ = Transicion::Ninguna;
};

} // namespace vp
