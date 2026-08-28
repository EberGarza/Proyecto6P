#include "Boton.hpp"

#include <algorithm>
#include <utility>

namespace vp {
namespace {

constexpr float kElevacionHover = 3.f;
constexpr float kVelocidadHover = 16.f;

}

Boton::Boton(const sf::Font& fuente, std::string etiqueta,
             sf::Vector2f posicion, sf::Vector2f tamano, Accion accion)
    : texto_(fuente, etiqueta, tema::kTextoNormal)
    , tecla_(fuente, "", 12)
    , accion_(std::move(accion))
{
    forma_.setSize(tamano);
    forma_.setPosition(posicion);
    forma_.setFillColor(colorBase_);
    forma_.setOutlineThickness(2.f);
    forma_.setOutlineColor(tema::kPanelBorde);

    filoSuperior_.setSize({ tamano.x, 2.f });
    filoSuperior_.setPosition(posicion);
    filoIzquierdo_.setSize({ 2.f, tamano.y });
    filoIzquierdo_.setPosition(posicion);

    marcaTecla_.setSize({ 18.f, 16.f });
    marcaTecla_.setPosition({ posicion.x + 4.f, posicion.y + 4.f });
    marcaTecla_.setFillColor(tema::kPanelBorde);

    texto_.setFillColor(tema::kTexto);
    tecla_.setFillColor(tema::kAcento);

    centrarTexto();
    refrescarColor();
}

void Boton::establecerTecla(const std::string& etiquetaTecla)
{
    tecla_.setString(etiquetaTecla);

    const sf::FloatRect limites = tecla_.getLocalBounds();
    const sf::Vector2f  marca   = marcaTecla_.getPosition();
    tecla_.setOrigin({ limites.position.x, limites.position.y });
    tecla_.setPosition({ marca.x + (18.f - limites.size.x) / 2.f, marca.y + 1.f });
}

void Boton::centrarTexto()
{
    const sf::FloatRect limites = texto_.getLocalBounds();
    texto_.setOrigin({ limites.position.x + limites.size.x * 0.5f,
                       limites.position.y + limites.size.y * 0.5f });

    const sf::Vector2f p = forma_.getPosition();
    const sf::Vector2f t = forma_.getSize();

    texto_.setPosition({ p.x + t.x * 0.5f, p.y + t.y * 0.5f + 6.f });
}

void Boton::actualizar(sf::Vector2f posicionRaton)
{
    resaltado_ = habilitado_ && disponible_ && contiene(posicionRaton);
    refrescarColor();

    const float dt = std::min(relojHover_.restart().asSeconds(), 0.2f);
    const float objetivo = resaltado_ ? kElevacionHover : 0.f;
    desplazamientoHover_ += (objetivo - desplazamientoHover_) * std::min(1.f, dt * kVelocidadHover);
}

bool Boton::procesarClic(sf::Vector2f posicionClic)
{
    if (!habilitado_ || !contiene(posicionClic)) return false;

    return accionar();
}

bool Boton::accionar()
{
    if (!habilitado_) return false;

    if (accion_) accion_();
    return true;
}

void Boton::establecerHabilitado(bool habilitado)
{
    habilitado_ = habilitado;
    refrescarColor();
}

void Boton::establecerDisponible(bool disponible)
{
    disponible_ = disponible;
    refrescarColor();
}

void Boton::establecerEtiqueta(const std::string& etiqueta)
{
    if (texto_.getString() == etiqueta) return;

    texto_.setString(etiqueta);
    centrarTexto();
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

    if (!habilitado_ || !disponible_)
    {
        forma_.setFillColor(tema::kBotonApagado);
        forma_.setOutlineColor(tema::kPanelBorde);
        texto_.setFillColor(tema::kTextoTenue);
        tecla_.setFillColor(tema::kTextoTenue);
        filoSuperior_.setFillColor(tema::conAlfa(tema::kBiselClaro, 60));
        filoIzquierdo_.setFillColor(tema::conAlfa(tema::kBiselClaro, 60));
        return;
    }

    texto_.setFillColor(tema::kTexto);
    tecla_.setFillColor(tema::kAcento);
    forma_.setFillColor(resaltado_ ? colorResaltado_ : colorBase_);

    forma_.setOutlineColor((resaltado_ || activo_) ? tema::kAcento : tema::kPanelBorde);

    filoSuperior_.setFillColor(tema::kBiselClaro);
    filoIzquierdo_.setFillColor(tema::kBiselClaro);
}

bool Boton::contiene(sf::Vector2f punto) const
{
    return forma_.getGlobalBounds().contains(punto);
}

void Boton::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    sf::RenderStates propios = estados;
    propios.transform.translate({ 0.f, -desplazamientoHover_ });

    objetivo.draw(forma_, propios);
    objetivo.draw(filoSuperior_, propios);
    objetivo.draw(filoIzquierdo_, propios);

    if (!tecla_.getString().isEmpty())
    {
        objetivo.draw(marcaTecla_, propios);
        objetivo.draw(tecla_, propios);
    }

    tema::dibujarConSombra(objetivo, texto_, 2.f, propios);
}

}
