#include "VistaMascota.hpp"

#include "RenderProcedural.hpp"
#include "RenderSprite.hpp"
#include "Mascota.hpp"

#include <utility>

namespace vp {

VistaMascota::VistaMascota(const sf::Font& fuente)
    : fuente_(fuente)
    , render_(std::make_unique<RenderProcedural>(fuente))
{
}

bool VistaMascota::prepararEspecie(const std::string& especie)
{
    auto conSprites = std::make_unique<RenderSprite>();

    if (conSprites->cargar(especie))
    {
        render_        = std::move(conSprites);
        usandoSprites_ = true;
    }
    else
    {
        // Sin arte disponible se dibuja con figuras: el juego sigue siendo
        // jugable y presentable aunque assets/images/ este vacia.
        render_        = std::make_unique<RenderProcedural>(fuente_);
        usandoSprites_ = false;
    }

    render_->establecerEscala(escala_);
    render_->establecerPosicion(posicion_);
    return usandoSprites_;
}

void VistaMascota::actualizar(const Mascota& mascota, float dt)
{
    if (render_) render_->actualizar(mascota, dt);
}

void VistaMascota::establecerPosicion(sf::Vector2f posicion)
{
    posicion_ = posicion;
    if (render_) render_->establecerPosicion(posicion);
}

void VistaMascota::establecerEscala(float escala)
{
    escala_ = escala;
    if (render_) render_->establecerEscala(escala);
}

void VistaMascota::forzarProcedural()
{
    render_        = std::make_unique<RenderProcedural>(fuente_);
    usandoSprites_ = false;
    render_->establecerEscala(escala_);
    render_->establecerPosicion(posicion_);
}

const char* VistaMascota::nombreRender() const
{
    return render_ ? render_->nombreRender() : "ninguno";
}

void VistaMascota::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    if (render_) objetivo.draw(*render_, estados);
}

} // namespace vp
