#pragma once

#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

namespace vp {

class Atributo;

/**
 * @brief Barra de estado con aspecto de marcador de recreativa.
 *
 * Toma prestados tres recursos de las barras de vida de los juegos de pelea:
 *
 *  - **Forma inclinada.** Un paralelogramo en vez de un rectangulo.
 *  - **Muescas.** Lineas verticales que segmentan la barra, para que se lea el
 *    nivel de un vistazo sin tener que mirar el numero.
 *  - **Rastro.** Cuando el valor baja de golpe, un bloque rojo se queda atras y
 *    va cayendo despues. Es lo que hace que un golpe se *vea*, y aqui sirve
 *    igual: si la mascota pierde salud de repente, se nota.
 *
 * La barra se vacia de derecha a izquierda, como es costumbre en el genero.
 */
class BarraAtributo : public sf::Drawable
{
public:
    BarraAtributo(const sf::Font& fuente, std::string etiqueta,
                  sf::Vector2f posicion, sf::Vector2f tamano);

    /// Sincroniza el nivel con el atributo. dt mueve el rastro.
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

    float nivel_  = 1.f;   ///< valor real, de 0 a 1
    float rastro_ = 1.f;   ///< valor que persigue al real, siempre por detras
};

} // namespace vp
