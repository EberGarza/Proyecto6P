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

}

std::string RenderSprite::rutaDe(const std::string& clave)
{
    return "assets/images/" + enMinusculas(clave) + ".txt";
}

bool RenderSprite::cargar(const std::string& clave)
{
    listo_ = false;
    sprite_.reset();

    if (!hoja_.cargar(rutaDe(clave))) return false;

    const Animacion* base = hoja_.animacion(TipoEstado::Normal);
    if (!base) return false;

    animacion_      = *base;
    estadoDibujado_ = TipoEstado::Normal;
    escalaHoja_     = hoja_.escala();

    sprite_.emplace(hoja_.textura());
    sprite_->setColor(hoja_.tinte(TipoEstado::Normal));
    animacion_.aplicarCuadroActual(*sprite_);

    listo_ = true;
    aplicarEscala();
    return true;
}

void RenderSprite::actualizar(const Mascota& mascota, float dt)
{
    if (!listo_ || !sprite_) return;

    const TipoEstado tipo       = mascota.tipoEstado();
    const Actividad  actividad  = mascota.actividad();

    if (tipo != estadoDibujado_ || actividad != actividadDibujada_)
    {
        estadoDibujado_    = tipo;
        actividadDibujada_ = actividad;

        const std::string nombreAccion = animacionDeActividad(actividad);
        const Animacion*  elegida      = nombreAccion.empty() ? nullptr
                                                              : hoja_.accion(nombreAccion);

        if (!elegida) elegida = hoja_.animacion(tipo);
        if (!elegida) elegida = hoja_.animacion(TipoEstado::Normal);

        ponerAnimacion(elegida, hoja_.tinte(tipo));
    }

    animacion_.actualizar(*sprite_, dt);

    ajustarAPixel();
}

void RenderSprite::ponerAnimacion(const Animacion* nueva, sf::Color tinte)
{
    if (!sprite_) return;

    if (nueva) animacion_ = *nueva;
    animacion_.reiniciar();
    sprite_->setColor(tinte);

    animacion_.aplicarCuadroActual(*sprite_);
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

    const float total = escalaHoja_ * escalaExterna_;
    sprite_->setScale({ total, total });
    ajustarAPixel();
}

void RenderSprite::ajustarAPixel()
{
    if (!sprite_) return;

    const float total = escalaHoja_ * escalaExterna_;
    const sf::Vector2f desplazamiento = sprite_->getOrigin() * total;

    sprite_->setPosition({ std::round(posicion_.x - desplazamiento.x) + desplazamiento.x,
                           std::round(posicion_.y - desplazamiento.y) + desplazamiento.y });
}

void RenderSprite::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    if (listo_ && sprite_) objetivo.draw(*sprite_, estados);
}

}
