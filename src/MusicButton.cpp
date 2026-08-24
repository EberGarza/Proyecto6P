#include "MusicButton.hpp"

namespace vp {

bool MusicButton::cargar(const std::string& ruta, sf::Vector2f posicion, float escala)
{
    if (!textura_.loadFromFile(ruta)) return false;

    textura_.setSmooth(true);   // el icono no es pixel art: aqui si conviene suavizar

    sprite_.emplace(textura_);
    sprite_->setScale({ escala, escala });
    sprite_->setPosition(posicion);
    establecerActivo(activo_);
    return true;
}

bool MusicButton::contiene(sf::Vector2f punto) const
{
    return sprite_ && sprite_->getGlobalBounds().contains(punto);
}

void MusicButton::establecerActivo(bool activo)
{
    activo_ = activo;
    if (!sprite_) return;

    // Apagado se dibuja translucido, sin necesidad de un segundo icono.
    sprite_->setColor(activo_ ? sf::Color(255, 255, 255, 255)
                              : sf::Color(255, 255, 255, 70));
}

void MusicButton::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    if (sprite_) objetivo.draw(*sprite_, estados);
}

} // namespace vp
