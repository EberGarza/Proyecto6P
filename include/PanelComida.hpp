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

/**
 * @brief Cartelera de comida: elige que se le da de comer a la mascota.
 *
 * Se abre al pulsar ALIMENTAR y muestra lo que hay en el inventario, con lo
 * que alimenta cada cosa y cuantas raciones quedan. Antes el boton daba
 * siempre la misma cantidad fija; ahora cada alimento vale lo suyo, y esa
 * cantidad es la que decide cuanto dura la animacion de comer.
 *
 * No es dueno de nada: pinta lo que le pasa el Inventario y avisa por una
 * funcion cuando el jugador elige. Quien decide que hacer es PantallaJuego.
 */
class PanelComida : public sf::Drawable
{
public:
    /// Se llama con el indice del objeto elegido dentro del inventario.
    using AlElegir = std::function<void(std::size_t)>;

    PanelComida(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    /// Rehace la lista con la comida disponible. Llamar al abrir.
    void refrescar(const Inventario& inventario, AlElegir alElegir);

    void abrir()  { visible_ = true; }
    void cerrar() { visible_ = false; }
    bool visible() const { return visible_; }

    void actualizar(sf::Vector2f raton);

    /// Devuelve true si el clic fue para este panel (aunque no diera a nada).
    bool procesarClic(sf::Vector2f punto);

    /// Elige por numero de tecla (1..n). true si habia algo en esa posicion.
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

} // namespace vp
