#pragma once

#include "BarraAtributo.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

namespace vp {

class Mascota;

/**
 * @brief Marcador superior, al estilo de un juego de pelea de recreativa.
 *
 * La distribucion imita la de esos marcadores: arriba del todo la chapa con el
 * nombre a la izquierda y el estado a la derecha; debajo una barra de SALUD
 * ancha que cruza la pantalla, y bajo ella cuatro medidores secundarios en
 * fila. Es la misma informacion que antes, pero ordenada por importancia en
 * vez de en una lista.
 *
 * La bitacora queda como un teletipo translucido en la parte baja, para no
 * robarle sitio a la mascota.
 */
class Hud : public sf::Drawable
{
public:
    Hud(const sf::Font& fuente, sf::Vector2f tamanoVentana, float alturaTira);

    void actualizar(const Mascota& mascota, float dt);

    /// Alto que ocupa la tira superior, para que la pantalla coloque el resto.
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

    sf::RectangleShape fondoBitacora_;

    /// Orden fijo: salud, saciedad, felicidad, energia, higiene.
    std::vector<BarraAtributo> barras_;

    static constexpr std::size_t kLineasBitacora = 4;
};

} // namespace vp
