#pragma once

#include "RenderMascota.hpp"
#include "TipoEstado.hpp"

#include <SFML/Graphics.hpp>

#include <string>

namespace vp {

/**
 * @brief Dibuja la mascota con figuras geometricas, sin necesidad de sprites.
 *
 * Es la estrategia por defecto: gracias a ella el juego se ve y se puede
 * entregar aunque la carpeta assets/images este vacia. La forma, el color y
 * la expresion cambian segun la especie y el estado, que es justo lo que pide
 * el diagrama de estados.
 */
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

    /// Colores y forma de orejas propios de cada especie.
    void configurarEspecie(const std::string& especie);

    /// Expresion, color y texto flotante que corresponden al estado.
    void aplicarEstado(TipoEstado tipo);

    /// Recoloca todas las piezas alrededor del centro, con el rebote aplicado.
    void reposicionar(float desplazamientoY);

    /// Reconstruye la curva de la boca. curvatura: -1 triste, +1 sonriente.
    void construirBoca(sf::Vector2f centro, float curvatura);

    // --- Piezas del dibujo ---
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

    // --- Estado del dibujo ---
    sf::Vector2f posicion_      { 0.f, 0.f };
    float        escala_        = 1.f;
    float        tiempo_        = 0.f;   ///< reloj propio de la animacion
    float        relojParpadeo_ = 0.f;
    float        aperturaOjos_  = 1.f;   ///< 1 abiertos, 0 cerrados
    float        curvaturaBoca_ = 0.4f;
    float        velocidadBote_ = 2.f;
    float        alturaBote_    = 4.f;
    float        giroCola_      = 0.f;

    sf::Color colorCuerpo_ { 190, 150, 105 };
    sf::Color colorPanza_  { 232, 208, 176 };
    sf::Color tinteEstado_ = sf::Color::White;

    TipoEstado  estado_       = TipoEstado::Normal;
    std::string especieActual_;

    // --- Medidas base, antes de aplicar la escala ---
    static constexpr float kRadioCuerpo = 60.f;
    static constexpr float kRadioPanza  = 42.f;
    static constexpr float kRadioOreja  = 22.f;
    static constexpr float kRadioOjo    = 13.f;
    static constexpr float kRadioPupila = 6.f;
    static constexpr float kRadioHocico = 10.f;
    static constexpr int   kPuntosBoca  = 11;
};

} // namespace vp
