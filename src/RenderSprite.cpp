#include "RenderSprite.hpp"

#include "Mascota.hpp"

#include <cctype>
#include <cmath>

namespace vp {
namespace {

std::string enMinusculas(std::string texto)
{
    for (char& c : texto)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return texto;
}

} // namespace sin nombre

std::string RenderSprite::rutaDe(const std::string& especie)
{
    return "assets/images/" + enMinusculas(especie) + ".txt";
}

bool RenderSprite::cargar(const std::string& especie)
{
    listo_ = false;
    sprite_.reset();

    if (!hoja_.cargar(rutaDe(especie))) return false;

    // Sin animacion de Normal no hay respaldo para los estados sin dibujo.
    const Animacion* base = hoja_.animacion(TipoEstado::Normal);
    if (!base) return false;

    animacion_      = *base;
    estadoDibujado_ = TipoEstado::Normal;
    escalaHoja_     = hoja_.escala();

    // Ahora si existe la textura, asi que ya se puede construir el sprite.
    sprite_.emplace(hoja_.textura());
    sprite_->setColor(hoja_.tinte(TipoEstado::Normal));
    animacion_.aplicarCuadroActual(*sprite_);

    listo_ = true;
    aplicarEscala();   // ya alinea a pixel
    return true;
}

void RenderSprite::actualizar(const Mascota& mascota, float dt)
{
    if (!listo_ || !sprite_) return;

    const TipoEstado tipo = mascota.tipoEstado();

    if (tipo != estadoDibujado_)
    {
        estadoDibujado_ = tipo;

        // Si el estado no tiene dibujo propio se recurre al de Normal; el
        // tinte declarado en el archivo se encarga de diferenciarlo.
        const Animacion* nueva = hoja_.animacion(tipo);
        if (!nueva) nueva = hoja_.animacion(TipoEstado::Normal);
        if (nueva) animacion_ = *nueva;

        animacion_.reiniciar();
        sprite_->setColor(hoja_.tinte(tipo));
    }

    animacion_.actualizar(*sprite_, dt);

    // El cuadro que acaba de aplicarse trae su propio origen, asi que hay que
    // recolocar el sprite en la rejilla de pixeles despues de cada cambio.
    ajustarAPixel();
}

void RenderSprite::establecerPosicion(sf::Vector2f posicion)
{
    posicion_ = posicion;
    ajustarAPixel();
}

void RenderSprite::establecerEscala(float escala)
{
    escalaExterna_ = (escala > 0.f) ? escala : 1.f;
    aplicarEscala();
}

void RenderSprite::aplicarEscala()
{
    if (!sprite_) return;

    // La hoja trae su propia escala (el pixel art suele necesitar agrandarse)
    // y la pantalla aporta la suya. Se multiplican.
    const float total = escalaHoja_ * escalaExterna_;
    sprite_->setScale({ total, total });
    ajustarAPixel();
}

void RenderSprite::ajustarAPixel()
{
    if (!sprite_) return;

    // Los anclajes son fraccionarios (se midieron sobre la cara) y la posicion
    // del escenario tambien lo es. Si la esquina de la textura cae entre dos
    // pixeles, el muestreo por vecino mas cercano hace que una fila o columna
    // se desplace de un fotograma a otro: es el hormigueo tipico del pixel art.
    //
    // Se redondea la ESQUINA, no la posicion, para no perder el anclaje: asi la
    // textura arranca siempre en un pixel entero y la cara sigue en su sitio.
    const float total = escalaHoja_ * escalaExterna_;
    const sf::Vector2f desplazamiento = sprite_->getOrigin() * total;

    sprite_->setPosition({ std::round(posicion_.x - desplazamiento.x) + desplazamiento.x,
                           std::round(posicion_.y - desplazamiento.y) + desplazamiento.y });
}

void RenderSprite::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    if (listo_ && sprite_) objetivo.draw(*sprite_, estados);
}

} // namespace vp
