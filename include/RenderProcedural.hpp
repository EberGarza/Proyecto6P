#pragma once

#include "RenderMascota.hpp"
#include "TipoEstado.hpp"

#include <SFML/Graphics.hpp>

#include <string>

namespace vp {

class RenderProcedural : public RenderMascota
{
public:
    explicit RenderProcedural(const sf::Font& fuente);

    void actualizar(const Mascota& mascota, float dt) override;
    void establecerPosicion(sf::Vector2f posicion) override;
    void establecerEscala(float escala) override;

    const char* nombreRender() const override { return "Procedural"; }

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    void configurarEspecie(const std::string& especie);

    void aplicarEstado(TipoEstado tipo);

    void reposicionar(float desplazamientoY);

    void construirBoca(sf::Vector2f centro, float curvatura);

    sf::CircleShape    cuerpo_;
    sf::CircleShape    panza_;
    sf::CircleShape    orejaIzq_;
    sf::CircleShape    orejaDer_;
    sf::CircleShape    ojoIzq_;
    sf::CircleShape    ojoDer_;
    sf::CircleShape    pupilaIzq_;
    sf::CircleShape    pupilaDer_;
    sf::CircleShape    hocico_;
    sf::RectangleShape parpadoIzq_;
    sf::RectangleShape parpadoDer_;
    sf::RectangleShape cola_;
    sf::VertexArray    boca_;
    sf::Text           efecto_;

    sf::Vector2f posicion_      { 0.f, 0.f };
    float        escala_        = 1.f;
    float        tiempo_        = 0.f;
    float        relojParpadeo_ = 0.f;
    float        aperturaOjos_  = 1.f;
    float        curvaturaBoca_ = 0.4f;
    float        velocidadBote_ = 2.f;
    float        alturaBote_    = 4.f;
    float        giroCola_      = 0.f;

    sf::Color colorCuerpo_ { 190, 150, 105 };
    sf::Color colorPanza_  { 232, 208, 176 };
    sf::Color tinteEstado_ = sf::Color::White;

    TipoEstado  estado_       = TipoEstado::Normal;
    std::string especieActual_;

    static constexpr float kRadioCuerpo = 60.f;
    static constexpr float kRadioPanza  = 42.f;
    static constexpr float kRadioOreja  = 22.f;
    static constexpr float kRadioOjo    = 13.f;
    static constexpr float kRadioPupila = 6.f;
    static constexpr float kRadioHocico = 10.f;
    static constexpr int   kPuntosBoca  = 11;
};

}
