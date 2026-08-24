#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace vp {

/**
 * @brief Fondo de mosaico: la mascota repetida en tablero, muy tenue.
 *
 * Es el fondo que comparten el menu, las opciones y la partida. Estaba escrito
 * tres veces, una por pantalla, y bastaba con que alguien tocara el paso o la
 * transparencia de una para que dejaran de parecer el mismo juego. Ahora es una
 * sola funcion y las tres piden lo mismo.
 *
 * Se compone UNA vez sobre una textura y despues se dibuja como un unico
 * sprite: repetir cientos de sprites en cada fotograma seria tirar el tiempo.
 *
 * @param lienzo   Textura donde se compone. La funcion la redimensiona.
 * @param tamano   Tamano en pixeles del fondo.
 * @param hojas    Rutas de los .txt de sprites que se alternan en tablero.
 *                 Las que no carguen se saltan; si no carga ninguna, queda
 *                 solo el color de fondo y el juego sigue funcionando.
 * @param base     Color de fondo sobre el que se estampa el patron.
 * @param alfa     Transparencia del sello. Bajo a proposito: es un fondo.
 * @param paso     Separacion entre sellos.
 * @param escala   Aumento de cada sello.
 *
 * @return false si el lienzo no se pudo redimensionar.
 */
bool componerMosaico(sf::RenderTexture& lienzo,
                     sf::Vector2f tamano,
                     const std::vector<std::string>& hojas,
                     sf::Color base,
                     std::uint8_t alfa = 26,
                     float paso        = 128.f,
                     float escala      = 2.f);

} // namespace vp
