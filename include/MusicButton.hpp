#ifndef MUSIC_BUTTON_HPP
#define MUSIC_BUTTON_HPP

#include <SFML/Graphics.hpp>

#include <optional>

namespace vp {

/**
 * @brief Icono de altavoz en una esquina, para silenciar la musica.
 *
 * Se apaga bajando la opacidad en vez de cambiar de imagen, asi basta con un
 * solo archivo. En SFML 3 sf::Sprite exige una textura en el constructor, y
 * aqui la textura no existe hasta que se carga el archivo, por eso el sprite
 * vive dentro de un std::optional.
 */
class MusicButton : public sf::Drawable
{
public:
    MusicButton() = default;

    /// Carga el icono y lo coloca. false si falta el archivo.
    bool cargar(const std::string& ruta, sf::Vector2f posicion, float escala = 0.08f);

    /// true si el punto cae dentro del icono.
    bool contiene(sf::Vector2f punto) const;

    /// Cambia el aspecto entre encendido y apagado.
    void establecerActivo(bool activo);
    bool activo() const { return activo_; }

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    sf::Texture               textura_;
    std::optional<sf::Sprite> sprite_;
    bool                      activo_ = true;
};

} // namespace vp

#endif // MUSIC_BUTTON_HPP
