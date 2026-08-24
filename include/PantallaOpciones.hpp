#ifndef PANTALLA_OPCIONES_HPP
#define PANTALLA_OPCIONES_HPP

#include "Music.hpp"
#include "Pantalla.hpp"
#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <optional>
#include <vector>

namespace vp {

/**
 * @brief Pantalla de opciones: musica, volumen y borrado de la partida.
 *
 * Se maneja igual que el menu principal: arriba y abajo para cambiar de fila,
 * izquierda y derecha para ajustar el valor, Enter para las acciones y Escape
 * para volver.
 *
 * Comparte el acabado del menu y de la partida: marquesina para el titulo,
 * placas inclinadas para las filas y barrido de tubo por encima. El volumen se
 * dibuja ademas con una barra segmentada, la misma forma que las barras de la
 * mascota, para que se entienda de un vistazo sin leer el numero.
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

    void construirInterfaz();
    void ajustar(int direccion);
    void activar();
    void refrescarTextos();

    const sf::Font& fuente_;
    sf::Vector2f    tamanoVentana_;

    /// Mismo fondo de mosaico que el menu, compuesto una sola vez.
    sf::RenderTexture         lienzoFondo_;
    std::optional<sf::Sprite> fondo_;
    sf::RectangleShape        velo_;

    tema::PanelBiselado marquesina_;
    sf::Text            titulo_;

    /// Cada fila es una placa con la etiqueta a la izquierda y el valor a la
    /// derecha, en vez de una sola linea centrada: asi los valores quedan
    /// alineados entre si y se leen en columna.
    std::vector<sf::ConvexShape> placas_;
    std::vector<sf::Text>        etiquetas_;
    std::vector<sf::Text>        valores_;

    sf::ConvexShape     senalador_;

    /// Barra segmentada del volumen, con la misma forma que las de la mascota.
    sf::ConvexShape     fondoVolumen_;
    sf::ConvexShape     barraVolumen_;
    sf::Vector2f        origenVolumen_ { 0.f, 0.f };
    float               anchoVolumen_  = 0.f;

    tema::PanelBiselado tiraPie_;
    sf::Text            pie_;
    sf::VertexArray     barrido_;

    Music musica_;

    std::size_t seleccion_    = 0;
    bool        musicaActiva_ = true;
    int         volumen_      = 45;
    bool        borrada_      = false;
    float       reloj_        = 0.f;
    sf::Vector2f raton_ { -1.f, -1.f };

    static constexpr float kAnchoPlaca = 460.f;
    static constexpr float kAltoPlaca  = 46.f;
    static constexpr float kHuecoPlaca = 14.f;
};

} // namespace vp

#endif // PANTALLA_OPCIONES_HPP
