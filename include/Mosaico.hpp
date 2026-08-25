#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace vp {

bool componerMosaico(sf::RenderTexture& lienzo,
                     sf::Vector2f tamano,
                     const std::vector<std::string>& hojas,
                     sf::Color base,
                     std::uint8_t alfa = 26,
                     float paso        = 128.f,
                     float escala      = 2.f);

}
