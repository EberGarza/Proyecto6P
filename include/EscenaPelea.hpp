#pragma once

#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <string>

namespace vp {

class Mascota;
class VistaMascota;

// Base del minijuego de pelea: se abre como una escena superpuesta dentro
// de PantallaJuego (mismo espiritu que PanelComida/PanelAdmin), reutiliza
// la VistaMascota real del jugador y enfrenta contra un rival dibujado con
// formas simples. Sin arte de pelea todavia -- la mecanica (moverse,
// golpear, vida, IA, recompensa) es la base sobre la que crecera despues.
class EscenaPelea : public sf::Drawable
{
public:
    EscenaPelea(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void abrir(Mascota& mascota, VistaMascota& vista);
    bool visible() const { return fase_ != Fase::Cerrada; }

    void manejarEvento(const sf::Event& evento);
    void actualizar(float dt);

private:
    enum class Fase { Cerrada, Presentacion, Combate, Resultado };

    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    void actualizarCombate(float dt);
    void actualizarIaRival(float dt);
    void golpearSiEnRango();
    void terminarCombate(bool gano);
    void anunciar(const std::string& texto, sf::Color color);
    void refrescarBarras();

    const sf::Font& fuente_;
    sf::Vector2f    tamanoVentana_;

    Mascota*      mascota_ = nullptr;
    VistaMascota* vista_   = nullptr;

    Fase  fase_      = Fase::Cerrada;
    float relojFase_ = 0.f;
    bool  gano_       = false;

    float xJugador_ = 0.f;
    float xRival_   = 0.f;
    float yPelea_   = 0.f;

    float vidaJugador_ = 100.f;
    float vidaRival_   = 100.f;

    float enfriamientoGolpeJugador_ = 0.f;
    float enfriamientoGolpeRival_   = 0.f;
    float avisoGolpeRival_          = 0.f;

    sf::RectangleShape fondo_;
    sf::VertexArray    barrido_;

    tema::PanelBiselado marquesina_;
    sf::Text            titulo_;

    sf::ConvexShape fondoVidaJugador_;
    sf::ConvexShape rellenoVidaJugador_;
    sf::ConvexShape fondoVidaRival_;
    sf::ConvexShape rellenoVidaRival_;
    sf::Text        etiquetaJugador_;
    sf::Text        etiquetaRival_;

    sf::CircleShape cuerpoRival_;
    sf::CircleShape cabezaRival_;

    tema::PanelBiselado tiraPie_;
    sf::Text            pie_;

    sf::Text  anuncio_;
    float     anuncioRestante_ = 0.f;

    static constexpr float kMargenArena         = 90.f;
    static constexpr float kRangoGolpe          = 95.f;
    static constexpr float kVelocidadJugador    = 260.f;
    static constexpr float kVelocidadRival      = 150.f;
    static constexpr float kDanoGolpeJugador    = 14.f;
    static constexpr float kDanoGolpeRival      = 10.f;
    static constexpr float kEnfriamientoJugador = 0.45f;
    static constexpr float kEnfriamientoRival   = 1.4f;
    static constexpr float kAvisoRival          = 0.5f;
    static constexpr float kDuracionPresentacion = 1.2f;
    static constexpr float kDuracionResultado    = 2.2f;

    static constexpr int kMonedasPorVictoria    = 10;
    static constexpr float kFelicidadPorVictoria = 8.f;
    static constexpr float kEnergiaPorPelea      = 12.f;
    static constexpr float kEnergiaExtraDerrota  = 10.f;
};

}
