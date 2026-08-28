#include "BarraAtributo.hpp"

#include "Atributo.hpp"
#include "Utilidades.hpp"

#include <utility>

namespace vp {

BarraAtributo::BarraAtributo(const sf::Font& fuente, std::string etiqueta,
                             sf::Vector2f posicion, sf::Vector2f tamano)
    : etiqueta_(std::move(etiqueta))
    , tamano_(tamano)
    , texto_(fuente, etiqueta_, tema::kTextoChico)
    , valor_(fuente, "", tema::kTextoChico)
{
    fondo_.setSize(tamano);
    fondo_.setPosition(posicion);
    fondo_.setFillColor(tema::kFondo);
    fondo_.setOutlineThickness(1.f);
    fondo_.setOutlineColor(tema::kPanelBorde);

    relleno_.setSize(tamano);
    relleno_.setPosition(posicion);
    relleno_.setFillColor(tema::kBien);

    texto_.setFillColor(tema::kTexto);
    valor_.setFillColor(tema::kTextoSuave);

    establecerPosicion(posicion);
}

void BarraAtributo::actualizar(const Atributo& atributo)
{
    const float porcentaje = atributo.porcentaje();

    relleno_.setSize({ tamano_.x * porcentaje, tamano_.y });

    if (colorAutomatico_)
        relleno_.setFillColor(tema::segunNivel(porcentaje));

    valor_.setString(util::aTexto(atributo.valor()));

    // El numero va fuera de la barra, a su derecha. Dentro quedaria en blanco
    // sobre verde cuando la barra esta llena y no se leeria.
    const sf::FloatRect limites = valor_.getLocalBounds();
    const sf::Vector2f  origen  = fondo_.getPosition();
    valor_.setPosition({ origen.x + tamano_.x + 10.f,
                         origen.y + tamano_.y * 0.5f - limites.size.y });
}

void BarraAtributo::establecerPosicion(sf::Vector2f posicion)
{
    fondo_.setPosition(posicion);
    relleno_.setPosition(posicion);

    const sf::FloatRect limites = texto_.getLocalBounds();
    texto_.setPosition({ posicion.x + 8.f,
                         posicion.y + tamano_.y * 0.5f - limites.size.y });
}

void BarraAtributo::establecerColorBase(sf::Color color)
{
    colorAutomatico_ = false;
    relleno_.setFillColor(color);
}

void BarraAtributo::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    objetivo.draw(fondo_,   estados);
    objetivo.draw(relleno_, estados);
    objetivo.draw(texto_,   estados);
    objetivo.draw(valor_,   estados);
}

} // namespace vp
