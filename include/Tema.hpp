#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>

namespace vp {

/**
 * @brief Paleta y medidas de la interfaz, en un solo sitio.
 *
 * Tener los colores centralizados evita numeros magicos repartidos por todo
 * el codigo de dibujo y permite cambiar el aspecto del juego tocando un unico
 * archivo.
 */
namespace tema {

// --- Fondo y paneles ---
// Morado oscuro, el mismo del menu de inicio, para que las pantallas no
// parezcan de dos juegos distintos.
inline const sf::Color kFondo        (24, 16, 36);
inline const sf::Color kPanel        (44, 30, 66);
inline const sf::Color kPanelBorde   (12, 8, 20);      ///< borde negro duro, estilo arcade
inline const sf::Color kSombra       (0, 0, 0, 160);

// Bisel de los paneles: claro arriba a la izquierda, oscuro abajo a la derecha.
inline const sf::Color kBiselClaro   (108, 78, 150);
inline const sf::Color kBiselOscuro  (22, 14, 34);

// --- Texto ---
inline const sf::Color kTexto        (245, 244, 250);
inline const sf::Color kTextoSuave   (176, 160, 200);
inline const sf::Color kTextoTenue   (118, 102, 144);

// --- Botones ---
inline const sf::Color kBoton        (62, 42, 92);
inline const sf::Color kBotonHover   (92, 64, 132);
inline const sf::Color kBotonPulsado (40, 26, 62);
inline const sf::Color kBotonApagado (40, 34, 50);

// --- Semaforo de las barras ---
// Saturado como una barra de vida de recreativa.
inline const sf::Color kBien         (120, 226, 96);
inline const sf::Color kMedio        (255, 206, 60);
inline const sf::Color kMal          (232, 62, 58);

// --- Acentos ---
inline const sf::Color kAcento       (255, 220, 80);   ///< el amarillo del menu
inline const sf::Color kAdmin        (255, 122, 58);   ///< color propio del Admin_Menu

// --- Barras de vida ---
inline const sf::Color kBarraFondo   (30, 18, 44);     ///< hueco vacio de la barra
inline const sf::Color kBarraEstela  (198, 44, 40);    ///< rastro rojo que persigue al valor
inline const sf::Color kBarraBrillo  (255, 255, 255, 60);

// --- Medidas ---
inline constexpr float kRadioPanel   = 0.f;            ///< arcade: esquinas rectas
inline constexpr float kMargen       = 18.f;
inline constexpr float kSesgo        = 10.f;           ///< inclinacion de las barras
inline constexpr unsigned kTextoTitulo = 22;
inline constexpr unsigned kTextoNormal = 16;
inline constexpr unsigned kTextoChico  = 13;

/// Color de una barra segun su nivel (1.0 = lleno).
inline sf::Color segunNivel(float porcentaje)
{
    if (porcentaje > 0.60f) return kBien;
    if (porcentaje > 0.30f) return kMedio;
    return kMal;
}

/// Mismo color con otra transparencia.
inline sf::Color conAlfa(sf::Color color, std::uint8_t alfa)
{
    color.a = alfa;
    return color;
}

/**
 * @brief Paralelogramo inclinado hacia la derecha.
 *
 * Es la forma de las barras de vida de las recreativas de pelea: el lado
 * superior va desplazado respecto al inferior, y eso solo le da al marcador un
 * aire de velocidad que un rectangulo recto no tiene.
 */
inline sf::ConvexShape paralelogramo(sf::Vector2f posicion, sf::Vector2f tamano,
                                     float sesgo = kSesgo)
{
    sf::ConvexShape forma(4);
    forma.setPoint(0, { posicion.x + sesgo,            posicion.y });
    forma.setPoint(1, { posicion.x + sesgo + tamano.x, posicion.y });
    forma.setPoint(2, { posicion.x + tamano.x,         posicion.y + tamano.y });
    forma.setPoint(3, { posicion.x,                    posicion.y + tamano.y });
    return forma;
}

/**
 * @brief Panel con bisel, como los marcos de las recreativas.
 *
 * Se compone de tres rectangulos: el borde negro, la cara, y dos filos claros
 * arriba y a la izquierda. Devuelve el conjunto ya listo para dibujar en orden.
 */
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

/**
 * @brief Rejilla de lineas oscuras que imita el barrido de un monitor de tubo.
 *
 * Se dibuja al final, encima de todo lo demas. Son vertices y no una textura,
 * asi que no gasta memoria de video ni se deforma si cambia la ventana.
 *
 * Vive aqui y no en una pantalla concreta porque lo usan las tres: si cada una
 * se lo montara por su cuenta, bastaria con que alguien tocara el paso de una
 * para que dejaran de parecer el mismo monitor.
 */
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

/// Dibuja un texto con sombra dura desplazada, como los marcadores arcade.
inline void dibujarConSombra(sf::RenderTarget& objetivo, const sf::Text& texto,
                             float desplazamiento = 2.f)
{
    sf::Text sombra = texto;
    sombra.setFillColor(sf::Color(0, 0, 0, 200));
    sombra.setPosition({ texto.getPosition().x + desplazamiento,
                         texto.getPosition().y + desplazamiento });
    objetivo.draw(sombra);
    objetivo.draw(texto);
}

} // namespace tema
} // namespace vp
