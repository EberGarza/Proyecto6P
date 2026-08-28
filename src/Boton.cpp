#include "Boton.hpp"

#include <utility>

namespace vp {

Boton::Boton(const sf::Font& fuente, std::string etiqueta,
             sf::Vector2f posicion, sf::Vector2f tamano, Accion accion)
    : texto_(fuente, etiqueta, tema::kTextoNormal)
    , accion_(std::move(accion))
{
    forma_.setSize(tamano);
    forma_.setPosition(posicion);
    forma_.setFillColor(colorBase_);
    forma_.setOutlineThickness(1.f);
    forma_.setOutlineColor(tema::kPanelBorde);

    texto_.setFillColor(tema::kTexto);
    centrarTexto();
}

void Boton::centrarTexto()
{
    // getLocalBounds() en SFML 3 devuelve position/size en vez de left/top/width/height.
    const sf::FloatRect limites = texto_.getLocalBounds();
    texto_.setOrigin({ limites.position.x + limites.size.x * 0.5f,
                       limites.position.y + limites.size.y * 0.5f });

    const sf::Vector2f p = forma_.getPosition();
    const sf::Vector2f t = forma_.getSize();
    texto_.setPosition({ p.x + t.x * 0.5f, p.y + t.y * 0.5f });
}

void Boton::actualizar(sf::Vector2f posicionRaton)
{
    resaltado_ = habilitado_ && contiene(posicionRaton);
    refrescarColor();
}

bool Boton::procesarClic(sf::Vector2f posicionClic)
{
    if (!habilitado_ || !contiene(posicionClic)) return false;

    if (accion_) accion_();
    return true;
}

void Boton::establecerHabilitado(bool habilitado)
{
    habilitado_ = habilitado;
    refrescarColor();
}

void Boton::establecerColores(sf::Color base, sf::Color resaltado)
{
    colorBase_      = base;
    colorResaltado_ = resaltado;
    refrescarColor();
}

void Boton::establecerActivo(bool activo)
{
    activo_ = activo;
    refrescarColor();
}

void Boton::establecerTamanoTexto(unsigned tamano)
{
    texto_.setCharacterSize(tamano);
    centrarTexto();
}

void Boton::refrescarColor()
{
    if (!habilitado_)
    {
        forma_.setFillColor(tema::kBotonApagado);
        forma_.setOutlineColor(tema::kPanelBorde);
        texto_.setFillColor(tema::kTextoTenue);
        return;
    }

    texto_.setFillColor(tema::kTexto);
    forma_.setFillColor(resaltado_ ? colorResaltado_ : colorBase_);
    forma_.setOutlineColor(activo_ ? tema::kAcento : tema::kPanelBorde);
    forma_.setOutlineThickness(activo_ ? 2.f : 1.f);
}

bool Boton::contiene(sf::Vector2f punto) const
{
    return forma_.getGlobalBounds().contains(punto);
}

void Boton::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    objetivo.draw(forma_, estados);
    objetivo.draw(texto_, estados);
}

} // namespace vp
