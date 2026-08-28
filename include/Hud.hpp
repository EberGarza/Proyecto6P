#pragma once

#include "BarraAtributo.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

namespace vp {

class Mascota;

class Hud : public sf::Drawable
{
public:
    Hud(const sf::Font& fuente, sf::Vector2f tamanoVentana, float alturaTira);

    void actualizar(const Mascota& mascota, float dt);

    static constexpr float kAlturaTira = 132.f;

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    sf::Vector2f tamanoVentana_;

    tema::PanelBiselado tira_;
    tema::PanelBiselado chapaNombre_;

    sf::Text nombre_;
    sf::Text especie_;
    sf::Text estado_;
    sf::Text bitacora_;
    sf::Text monedas_;
    sf::CircleShape iconoMoneda_;

    sf::RectangleShape fondoBitacora_;

    std::vector<BarraAtributo> barras_;

    std::size_t entradasBitacoraVistas_ = static_cast<std::size_t>(-1);
    std::string ultimaEntradaVista_;

    static constexpr std::size_t kLineasBitacora = 4;
};

}
