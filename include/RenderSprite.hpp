#pragma once

#include "Actividad.hpp"
#include "HojaSprites.hpp"
#include "RenderMascota.hpp"
#include "TipoEstado.hpp"

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>

namespace vp {

/**
 * @brief Dibuja la mascota a partir de una hoja de sprites.
 *
 * Busca assets/images/<clave>.txt, que describe la hoja y los recortes de
 * cada estado. La clave junta especie y genero ("conejo_macho"), porque cada
 * combinacion tiene su propio dibujo. Si un estado no tiene animacion propia,
 * se usa la de Normal con el tinte que indique el archivo.
 *
 * Lo que se dibuja sale de dos preguntas, en este orden:
 *
 *   1. ¿Esta haciendo algo? Si la hoja tiene animacion para esa actividad
 *      (comer, banarse), esa manda y se repite mientras dure.
 *   2. Si no, se dibuja la animacion de su estado.
 *
 * El render no recibe ordenes de "reproduce esto ahora": mira a la mascota en
 * cada fotograma y se sincroniza. Asi no hay forma de que la animacion y la
 * mascota se queden diciendo cosas distintas.
 *
 * Nota de SFML 3: sf::Sprite ya no se puede construir sin textura, y aqui la
 * textura no existe hasta que se lee el archivo. Por eso el sprite vive dentro
 * de un std::optional y se construye en cargar().
 */
class RenderSprite : public RenderMascota
{
public:
    RenderSprite() = default;

    /// Intenta cargar la hoja. `clave` es el nombre base del archivo, sin
    /// extension: "conejo_macho". false si no existe o esta mal.
    bool cargar(const std::string& clave);

    void actualizar(const Mascota& mascota, float dt) override;
    void establecerPosicion(sf::Vector2f posicion) override;
    void establecerEscala(float escala) override;

    const char* nombreRender() const override { return "Sprites"; }

    const std::string& error() const { return hoja_.error(); }

    /// Ruta del archivo que describe una hoja, a partir de su clave.
    static std::string rutaDe(const std::string& clave);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    /// Multiplica la escala de la hoja por la de la pantalla.
    void aplicarEscala();

    /// Coloca el sprite de forma que la textura arranque en un pixel entero.
    void ajustarAPixel();

    /// Cambia a la animacion indicada y reinicia su reloj.
    void ponerAnimacion(const Animacion* nueva, sf::Color tinte);

    HojaSprites               hoja_;
    Animacion                 animacion_;   ///< copia de trabajo, con su propio reloj
    std::optional<sf::Sprite> sprite_;
    sf::Vector2f              posicion_       { 0.f, 0.f };
    float                     escalaHoja_     = 1.f;
    float                     escalaExterna_  = 1.f;
    /// Lo ultimo que se dibujo, para no reiniciar la animacion cada fotograma.
    TipoEstado                estadoDibujado_    = TipoEstado::Normal;
    Actividad                 actividadDibujada_ = Actividad::Ninguna;
    bool                      listo_             = false;
};

} // namespace vp
