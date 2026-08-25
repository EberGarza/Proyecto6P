#pragma once

#include <SFML/Graphics.hpp>

namespace vp {

class Mascota;

class RenderMascota : public sf::Drawable
{
public:
    ~RenderMascota() override = default;

    virtual void actualizar(const Mascota& mascota, float dt) = 0;

    virtual void establecerPosicion(sf::Vector2f posicion) = 0;
    virtual void establecerEscala(float escala)            = 0;

    virtual const char* nombreRender() const = 0;
};

}
