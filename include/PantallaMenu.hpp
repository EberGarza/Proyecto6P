#ifndef PANTALLA_MENU_HPP
#define PANTALLA_MENU_HPP

#include "Music.hpp"
#include "MusicButton.hpp"
#include "Pantalla.hpp"
#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace vp {

enum class OpcionMenu
{
    Jugar = 0,
    Opciones,
    Salir
};

class PantallaMenu : public Pantalla
{
public:
    PantallaMenu(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

private:
    void construirFondo();
    void colocarOpciones();
    void elegir(std::size_t indice);
    void mover(int direccion);

    const sf::Font& fuente_;
    sf::Vector2f    tamanoVentana_;

    sf::RenderTexture         lienzoFondo_;
    std::optional<sf::Sprite> fondo_;
    sf::RectangleShape        velo_;

    tema::PanelBiselado   marquesina_;
    sf::Text              titulo_;

    std::vector<sf::ConvexShape> placas_;
    std::vector<sf::Text>        textosMenu_;

    sf::ConvexShape       senalador_;

    tema::PanelBiselado   tiraPie_;
    sf::Text              pie_;

    sf::VertexArray       barrido_;

    Music       musica_;
    MusicButton botonMusica_;

    std::size_t  seleccion_ = 0;
    sf::Vector2f raton_ { -1.f, -1.f };
    float        reloj_ = 0.f;

    static constexpr float kAnchoPlaca = 340.f;
    static constexpr float kAltoPlaca  = 52.f;
    static constexpr float kHuecoPlaca = 14.f;
};

}

#endif
