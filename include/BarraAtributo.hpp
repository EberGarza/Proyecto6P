#pragma once

#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

namespace vp {

class Atributo;

class BarraAtributo : public sf::Drawable
{
public:
    BarraAtributo(const sf::Font& fuente, std::string etiqueta,
                  sf::Vector2f posicion, sf::Vector2f tamano);

    void actualizar(const Atributo& atributo, float dt);

    void establecerPosicion(sf::Vector2f posicion);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;
    void rehacerFormas();

    std::string  etiqueta_;
    sf::Vector2f posicion_;
    sf::Vector2f tamano_;

    sf::ConvexShape                marco_;
    sf::ConvexShape                hueco_;
    sf::ConvexShape                estela_;
    sf::ConvexShape                relleno_;
    sf::ConvexShape                brillo_;
    std::vector<sf::ConvexShape>   muescas_;

    sf::Text texto_;
    sf::Text valor_;

    float nivel_  = 1.f;
    float rastro_ = 1.f;
};

}
