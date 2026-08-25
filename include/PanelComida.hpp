#pragma once

#include "Boton.hpp"
#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <functional>
#include <vector>

namespace vp {

class Inventario;
class Mascota;

class PanelComida : public sf::Drawable
{
public:

    using AlElegir = std::function<void(std::size_t)>;

    PanelComida(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void refrescar(const Inventario& inventario, AlElegir alElegir);

    void abrir()  { visible_ = true; }
    void cerrar() { visible_ = false; }
    bool visible() const { return visible_; }

    void actualizar(sf::Vector2f raton);

    bool procesarClic(sf::Vector2f punto);

    bool elegirPorIndice(std::size_t indice);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    const sf::Font&    fuente_;
    sf::Vector2f       tamanoVentana_;
    sf::Vector2f       origen_;

    sf::RectangleShape velo_;
    tema::PanelBiselado panel_;
    sf::Text           titulo_;
    sf::Text           aviso_;

    std::vector<Boton>   botones_;
    std::vector<sf::Text> detalles_;

    bool visible_ = false;

    static constexpr float kAncho = 420.f;
    static constexpr float kAlto  = 250.f;
};

}
