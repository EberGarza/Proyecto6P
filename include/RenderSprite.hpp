#pragma once

#include "Actividad.hpp"
#include "HojaSprites.hpp"
#include "RenderMascota.hpp"
#include "TipoEstado.hpp"

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>

namespace vp {

class RenderSprite : public RenderMascota
{
public:
    RenderSprite() = default;

    bool cargar(const std::string& clave);

    void actualizar(const Mascota& mascota, float dt) override;
    void establecerPosicion(sf::Vector2f posicion) override;
    void establecerEscala(float escala) override;

    const char* nombreRender() const override { return "Sprites"; }

    const std::string& error() const { return hoja_.error(); }

    static std::string rutaDe(const std::string& clave);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    void aplicarEscala();

    void ajustarAPixel();

    void ponerAnimacion(const Animacion* nueva, sf::Color tinte);

    HojaSprites               hoja_;
    Animacion                 animacion_;
    std::optional<sf::Sprite> sprite_;
    sf::Vector2f              posicion_       { 0.f, 0.f };
    float                     escalaHoja_     = 1.f;
    float                     escalaExterna_  = 1.f;

    TipoEstado                estadoDibujado_    = TipoEstado::Normal;
    Actividad                 actividadDibujada_ = Actividad::Ninguna;
    bool                      listo_             = false;
};

}
