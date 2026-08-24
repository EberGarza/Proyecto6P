#include "Boton.hpp"

#include <utility>

namespace vp {

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

    // Bisel: un filo claro arriba y otro a la izquierda, como una tecla fisica.
    filoSuperior_.setSize({ tamano.x, 2.f });
    filoSuperior_.setPosition(posicion);
    filoIzquierdo_.setSize({ 2.f, tamano.y });
    filoIzquierdo_.setPosition(posicion);

    // Cuadrito con el numero de tecla, arriba a la izquierda del boton.
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
    // Un poco por debajo del centro, para dejar sitio al numero de tecla.
    texto_.setPosition({ p.x + t.x * 0.5f, p.y + t.y * 0.5f + 6.f });
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
        tecla_.setFillColor(tema::kTextoTenue);
        filoSuperior_.setFillColor(tema::conAlfa(tema::kBiselClaro, 60));
        filoIzquierdo_.setFillColor(tema::conAlfa(tema::kBiselClaro, 60));
        return;
    }

    texto_.setFillColor(tema::kTexto);
    tecla_.setFillColor(tema::kAcento);
    forma_.setFillColor(resaltado_ ? colorResaltado_ : colorBase_);

    // Al resaltar se enciende el borde en amarillo, como el boton pulsado de
    // una recreativa.
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
    objetivo.draw(forma_, estados);
    objetivo.draw(filoSuperior_, estados);
    objetivo.draw(filoIzquierdo_, estados);

    if (!tecla_.getString().isEmpty())
    {
        objetivo.draw(marcaTecla_, estados);
        objetivo.draw(tecla_, estados);
    }

    tema::dibujarConSombra(objetivo, texto_);
}

} // namespace vp
