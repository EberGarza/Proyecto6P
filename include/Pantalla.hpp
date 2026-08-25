#pragma once

#include <SFML/Graphics.hpp>

namespace vp {

class Pantalla
{
public:

    enum class Transicion
    {
        Ninguna,
        Menu,
        Opciones,
        Seleccion,
        Juego,
        Salir
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

}
