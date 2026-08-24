#ifndef PANTALLA_OPCIONES_HPP
#define PANTALLA_OPCIONES_HPP

#include "Music.hpp"
#include "Pantalla.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

namespace vp {

/**
 * @brief Pantalla de opciones: musica, volumen y borrado de la partida.
 *
 * Se maneja igual que el menu principal: arriba y abajo para cambiar de fila,
 * izquierda y derecha para ajustar el valor, Enter para las acciones y Escape
 * para volver.
 */
class PantallaOpciones : public Pantalla
{
public:
    PantallaOpciones(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

private:
    enum class Fila
    {
        Musica = 0,
        Volumen,
        BorrarPartida,
        Volver,
        Total
    };

    void ajustar(int direccion);
    void activar();
    void refrescarTextos();

    const sf::Font& fuente_;
    sf::Vector2f    tamanoVentana_;

    sf::RectangleShape    velo_;
    sf::Text              titulo_;
    sf::Text              pie_;
    std::vector<sf::Text> filas_;

    Music musica_;

    std::size_t seleccion_    = 0;
    bool        musicaActiva_ = true;
    int         volumen_      = 45;
    bool        borrada_      = false;
};

} // namespace vp

#endif // PANTALLA_OPCIONES_HPP
