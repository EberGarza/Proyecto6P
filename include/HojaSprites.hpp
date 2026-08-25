#pragma once

#include "Animacion.hpp"
#include "TipoEstado.hpp"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

namespace vp {

class HojaSprites
{
public:
    HojaSprites() = default;

    HojaSprites(const HojaSprites&)            = delete;
    HojaSprites& operator=(const HojaSprites&) = delete;
    HojaSprites(HojaSprites&&)                 = delete;
    HojaSprites& operator=(HojaSprites&&)      = delete;

    bool cargar(const std::string& rutaConfig);

    bool valida() const { return valida_; }

    const std::string& error() const { return error_; }

    const Animacion* animacion(TipoEstado tipo) const;

    sf::Color tinte(TipoEstado tipo) const;

    const Animacion* accion(const std::string& nombre) const;

    std::vector<std::string> accionesDisponibles() const;

    const sf::Texture& textura() const { return textura_; }
    float escala() const               { return escala_; }
    const std::string& nombre() const  { return nombre_; }

private:

    static void recortarFondo(sf::Image& imagen, sf::Color fondo, int tolerancia);

    static std::string carpetaDe(const std::string& ruta);

    sf::Texture                        textura_;
    std::map<TipoEstado, Animacion>    animaciones_;
    std::map<std::string, Animacion>   acciones_;
    std::map<TipoEstado, sf::Color>    tintes_;
    std::string                        nombre_;
    std::string                        error_;
    float                              escala_ = 1.f;
    bool                               valida_ = false;
};

}
