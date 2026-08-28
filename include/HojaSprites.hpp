#pragma once

#include "Animacion.hpp"
#include "TipoEstado.hpp"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

namespace vp {

/**
 * @brief Hoja de sprites descrita por un archivo de texto.
 *
 * Resuelve tres problemas de las hojas dibujadas a mano:
 *
 *  1. **Fondo opaco.** Si la hoja viene en JPG no trae canal alfa, asi que la
 *     mascota se veria dentro de un rectangulo de color. Al cargar se recorre
 *     la imagen y se vuelve transparente todo pixel parecido al color de
 *     fondo, con una tolerancia (el JPG comprime con perdida y el fondo no es
 *     un color perfectamente uniforme).
 *
 *  2. **Recortes irregulares.** Las poses no estan en una rejilla: cada una
 *     tiene su tamano y su sitio. Por eso los cuadros se declaran uno a uno.
 *
 *  3. **Faltan poses.** No toda hoja trae dibujo para los ocho estados. Se
 *     puede declarar un tinte por estado para reutilizar otra animacion
 *     cambiandole el color (verdoso para Enferma, gris para Muerta).
 *
 * Al estar todo en un archivo de texto, se pueden ajustar las coordenadas sin
 * recompilar: basta reiniciar el juego.
 */
class HojaSprites
{
public:
    HojaSprites() = default;

    // No se copia ni se mueve: cada Animacion guarda un puntero a textura_,
    // que es un miembro. Al copiar el objeto, esos punteros seguirian
    // apuntando a la textura del original.
    HojaSprites(const HojaSprites&)            = delete;
    HojaSprites& operator=(const HojaSprites&) = delete;
    HojaSprites(HojaSprites&&)                 = delete;
    HojaSprites& operator=(HojaSprites&&)      = delete;

    /// Lee el archivo de configuracion y carga la imagen que este indique.
    bool cargar(const std::string& rutaConfig);

    bool valida() const { return valida_; }

    /// Mensaje del ultimo fallo, para mostrarlo por consola.
    const std::string& error() const { return error_; }

    /// Animacion del estado, o nullptr si esa hoja no la define.
    const Animacion* animacion(TipoEstado tipo) const;

    /// Color por el que se multiplica el sprite en ese estado.
    sf::Color tinte(TipoEstado tipo) const;

    /// Animaciones del archivo cuyo nombre no es un estado del juego.
    /// Hoy se ignoran; sirven para avisar de que hay arte sin conectar.
    const std::vector<std::string>& ignoradas() const { return ignoradas_; }

    const sf::Texture& textura() const { return textura_; }
    float escala() const               { return escala_; }
    const std::string& nombre() const  { return nombre_; }

private:
    /// Convierte en transparente todo pixel cercano al color de fondo.
    static void recortarFondo(sf::Image& imagen, sf::Color fondo, int tolerancia);

    /// Carpeta que contiene el archivo de configuracion.
    static std::string carpetaDe(const std::string& ruta);

    sf::Texture                     textura_;
    std::map<TipoEstado, Animacion> animaciones_;
    std::map<TipoEstado, sf::Color> tintes_;
    std::string                     nombre_;
    std::string                     error_;
    std::vector<std::string>        ignoradas_;
    float                           escala_ = 1.f;
    bool                            valida_ = false;
};

} // namespace vp
