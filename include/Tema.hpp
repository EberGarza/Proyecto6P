#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>

namespace vp {

namespace tema {

inline const sf::Color kFondo        (24, 16, 36);
inline const sf::Color kPanel        (44, 30, 66);
inline const sf::Color kPanelBorde   (12, 8, 20);
inline const sf::Color kSombra       (0, 0, 0, 160);

inline const sf::Color kBiselClaro   (108, 78, 150);
inline const sf::Color kBiselOscuro  (22, 14, 34);

inline const sf::Color kTexto        (245, 244, 250);
inline const sf::Color kTextoSuave   (176, 160, 200);
inline const sf::Color kTextoTenue   (118, 102, 144);

inline const sf::Color kBoton        (62, 42, 92);
inline const sf::Color kBotonHover   (92, 64, 132);
inline const sf::Color kBotonPulsado (40, 26, 62);
inline const sf::Color kBotonApagado (40, 34, 50);

inline const sf::Color kBien         (120, 226, 96);
inline const sf::Color kMedio        (255, 206, 60);
inline const sf::Color kMal          (232, 62, 58);

inline const sf::Color kAcento       (255, 220, 80);
inline const sf::Color kAdmin        (255, 122, 58);

inline const sf::Color kBarraFondo   (30, 18, 44);
inline const sf::Color kBarraEstela  (198, 44, 40);
inline const sf::Color kBarraBrillo  (255, 255, 255, 60);

inline const sf::Color kFondoMosaico (38, 26, 54);

inline constexpr float kRadioPanel   = 0.f;
inline constexpr float kMargen       = 18.f;
inline constexpr float kSesgo        = 10.f;
inline constexpr unsigned kTextoTitulo = 22;
inline constexpr unsigned kTextoNormal = 16;
inline constexpr unsigned kTextoChico  = 13;

inline sf::Color segunNivel(float porcentaje)
{
    if (porcentaje > 0.60f) return kBien;
    if (porcentaje > 0.30f) return kMedio;
    return kMal;
}

inline sf::Color conAlfa(sf::Color color, std::uint8_t alfa)
{
    color.a = alfa;
    return color;
}

inline void ponerParalelogramo(sf::ConvexShape& forma, sf::Vector2f posicion,
                               sf::Vector2f tamano, float sesgo = kSesgo)
{
    if (forma.getPointCount() != 4) forma.setPointCount(4);
    forma.setPoint(0, { posicion.x + sesgo,            posicion.y });
    forma.setPoint(1, { posicion.x + sesgo + tamano.x, posicion.y });
    forma.setPoint(2, { posicion.x + tamano.x,         posicion.y + tamano.y });
    forma.setPoint(3, { posicion.x,                    posicion.y + tamano.y });
}

inline sf::ConvexShape paralelogramo(sf::Vector2f posicion, sf::Vector2f tamano,
                                     float sesgo = kSesgo)
{
    sf::ConvexShape forma;
    ponerParalelogramo(forma, posicion, tamano, sesgo);
    return forma;
}

struct PanelBiselado
{
    sf::RectangleShape borde;
    sf::RectangleShape cara;
    sf::RectangleShape filoSuperior;
    sf::RectangleShape filoIzquierdo;

    void dibujar(sf::RenderTarget& objetivo) const
    {
        objetivo.draw(borde);
        objetivo.draw(cara);
        objetivo.draw(filoSuperior);
        objetivo.draw(filoIzquierdo);
    }
};

inline PanelBiselado panelBiselado(sf::Vector2f posicion, sf::Vector2f tamano,
                                   sf::Color relleno = kPanel)
{
    PanelBiselado panel;

    panel.borde.setPosition({ posicion.x - 2.f, posicion.y - 2.f });
    panel.borde.setSize({ tamano.x + 4.f, tamano.y + 4.f });
    panel.borde.setFillColor(kPanelBorde);

    panel.cara.setPosition(posicion);
    panel.cara.setSize(tamano);
    panel.cara.setFillColor(relleno);

    panel.filoSuperior.setPosition(posicion);
    panel.filoSuperior.setSize({ tamano.x, 2.f });
    panel.filoSuperior.setFillColor(kBiselClaro);

    panel.filoIzquierdo.setPosition(posicion);
    panel.filoIzquierdo.setSize({ 2.f, tamano.y });
    panel.filoIzquierdo.setFillColor(kBiselClaro);

    return panel;
}

inline sf::VertexArray barridoCRT(sf::Vector2f tamano, std::uint8_t intensidad = 46)
{
    sf::VertexArray lineas(sf::PrimitiveType::Lines);
    const sf::Color tinta(0, 0, 0, intensidad);

    for (float y = 0.f; y < tamano.y; y += 3.f)
    {
        lineas.append(sf::Vertex{ { 0.f, y },       tinta });
        lineas.append(sf::Vertex{ { tamano.x, y },  tinta });
    }
    return lineas;
}

inline sf::ConvexShape senaladorMenu(sf::Color color, sf::Vector2f tamano = { 16.f, 18.f })
{
    sf::ConvexShape senalador(3);
    senalador.setPoint(0, { 0.f,          0.f });
    senalador.setPoint(1, { tamano.x,     tamano.y * 0.5f });
    senalador.setPoint(2, { 0.f,          tamano.y });
    senalador.setFillColor(color);
    return senalador;
}

inline void dibujarConSombra(sf::RenderTarget& objetivo, const sf::Text& texto,
                             float desplazamiento = 2.f,
                             sf::RenderStates estados = sf::RenderStates::Default)
{
    sf::Text sombra = texto;
    sombra.setFillColor(sf::Color(0, 0, 0, 200));
    sombra.setPosition({ texto.getPosition().x + desplazamiento,
                         texto.getPosition().y + desplazamiento });
    objetivo.draw(sombra, estados);
    objetivo.draw(texto, estados);
}

}
}
