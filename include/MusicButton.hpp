#ifndef MUSIC_BUTTON_HPP
#define MUSIC_BUTTON_HPP

#include <SFML/Graphics.hpp>

#include <optional>

namespace vp {

class MusicButton : public sf::Drawable
{
public:
    MusicButton() = default;

    bool cargar(const std::string& ruta, sf::Vector2f posicion, float escala = 0.08f);

    bool contiene(sf::Vector2f punto) const;

    void establecerActivo(bool activo);
    bool activo() const { return activo_; }

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    sf::Texture               textura_;
    std::optional<sf::Sprite> sprite_;
    bool                      activo_ = true;
};

}

#endif
