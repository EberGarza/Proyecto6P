#pragma once

#include "BarraAtributo.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

namespace vp {

class Mascota;

/**
 * @brief Panel lateral con el nombre, el estado, las cinco barras y la bitacora.
 */
class Hud : public sf::Drawable
{
public:
    Hud(const sf::Font& fuente, sf::Vector2f origen, sf::Vector2f tamano);

    /// Vuelca los datos de la mascota en los widgets.
    void actualizar(const Mascota& mascota);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    sf::RectangleShape panel_;
    sf::Text           titulo_;
    sf::Text           subtitulo_;
    sf::Text           estado_;
    sf::Text           tituloBitacora_;
    sf::Text           bitacora_;

    /// Orden fijo: saciedad, felicidad, energia, higiene, salud.
    std::vector<BarraAtributo> barras_;

    static constexpr std::size_t kLineasBitacora = 6;
};

} // namespace vp
