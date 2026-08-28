#pragma once

#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <string>

namespace vp {

class Atributo;

/**
 * @brief Barra de progreso que dibuja un Atributo de la mascota.
 *
 * Muestra la etiqueta a la izquierda y el valor a la derecha. El color sigue
 * el semaforo del tema: verde alto, amarillo medio, rojo urgente.
 *
 * Nota de SFML 3: sf::Text ya no se puede construir vacio, necesita la fuente
 * en el constructor. Por eso los textos se inicializan en la lista de
 * inicializacion y el orden de los miembros importa.
 */
class BarraAtributo : public sf::Drawable
{
public:
    BarraAtributo(const sf::Font& fuente, std::string etiqueta,
                  sf::Vector2f posicion, sf::Vector2f tamano);

    /// Sincroniza el ancho y el color con el valor actual del atributo.
    void actualizar(const Atributo& atributo);

    void establecerPosicion(sf::Vector2f posicion);
    void establecerColorBase(sf::Color color);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    std::string        etiqueta_;
    sf::Vector2f       tamano_;
    sf::RectangleShape fondo_;
    sf::RectangleShape relleno_;
    sf::Text           texto_;
    sf::Text           valor_;
    bool               colorAutomatico_ = true;
};

} // namespace vp
