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

class PantallaOpciones : public Pantalla
{
public:
    PantallaOpciones(const sf::Font& fuente, sf::Vector2f tamanoVentana,
                     bool pantallaCompletaActiva);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

private:
    enum class Fila
    {
        Musica = 0,
        Volumen,
        PantallaCompleta,
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

    sf::RenderTexture         lienzoFondo_;
    std::optional<sf::Sprite> fondo_;
    sf::RectangleShape        velo_;

    tema::PanelBiselado marquesina_;
    sf::Text            titulo_;

    std::vector<sf::ConvexShape> placas_;
    std::vector<sf::Text>        etiquetas_;
    std::vector<sf::Text>        valores_;

    sf::ConvexShape     senalador_;

    sf::ConvexShape     fondoVolumen_;
    sf::ConvexShape     barraVolumen_;
    sf::Vector2f        origenVolumen_ { 0.f, 0.f };
    float               anchoVolumen_  = 0.f;

    tema::PanelBiselado tiraPie_;
    sf::Text            pie_;
    sf::VertexArray     barrido_;

    Music musica_;

    std::size_t seleccion_             = 0;
    bool        musicaActiva_          = true;
    int         volumen_               = 45;
    bool        pantallaCompletaActiva_ = false;
    bool        borrada_               = false;
    float       reloj_        = 0.f;
    sf::Vector2f raton_ { -1.f, -1.f };

    static constexpr float kAnchoPlaca = 460.f;
    static constexpr float kAltoPlaca  = 46.f;
    static constexpr float kHuecoPlaca = 14.f;
};

}

#endif
