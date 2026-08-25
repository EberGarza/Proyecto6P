#pragma once

#include "RenderMascota.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <string>

namespace vp {

class Mascota;

class VistaMascota : public sf::Drawable
{
public:
    explicit VistaMascota(const sf::Font& fuente);

    bool prepararMascota(const Mascota& mascota);

    void actualizar(const Mascota& mascota, float dt);
    void establecerPosicion(sf::Vector2f posicion);
    void establecerEscala(float escala);

    void forzarProcedural();

    bool        usandoSprites() const { return usandoSprites_; }
    const char* nombreRender() const;

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    const sf::Font&                fuente_;
    std::unique_ptr<RenderMascota> render_;
    sf::Vector2f                   posicion_ { 0.f, 0.f };
    float                          escala_   = 1.f;
    bool                           usandoSprites_ = false;
};

}
