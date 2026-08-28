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
inline const sf::Color kFondo        (24, 27, 38);
inline const sf::Color kPanel        (34, 39, 54);
inline const sf::Color kPanelBorde   (56, 64, 86);
inline const sf::Color kSombra       (0, 0, 0, 90);

// --- Texto ---
inline const sf::Color kTexto        (232, 234, 242);
inline const sf::Color kTextoSuave   (150, 158, 180);
inline const sf::Color kTextoTenue   (104, 112, 134);

// --- Botones ---
inline const sf::Color kBoton        (58, 72, 104);
inline const sf::Color kBotonHover   (82, 102, 145);
inline const sf::Color kBotonPulsado (44, 55, 80);
inline const sf::Color kBotonApagado (46, 50, 62);

// --- Semaforo de las barras ---
inline const sf::Color kBien         (94, 196, 124);
inline const sf::Color kMedio        (232, 189, 74);
inline const sf::Color kMal          (219, 88, 76);

// --- Acentos ---
inline const sf::Color kAcento       (108, 160, 240);
inline const sf::Color kAdmin        (226, 106, 62);   ///< color propio del Admin_Menu

// --- Medidas ---
inline constexpr float kRadioPanel   = 8.f;
inline constexpr float kMargen       = 24.f;
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

} // namespace tema
} // namespace vp
