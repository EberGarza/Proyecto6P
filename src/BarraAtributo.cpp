#include "BarraAtributo.hpp"

#include "Atributo.hpp"
#include "Utilidades.hpp"

#include <utility>

namespace vp {
namespace {

constexpr float kAnchoMuesca = 2.f;
constexpr float kPasoMuesca  = 26.f;
constexpr float kCaidaRastro = 0.55f;

}

BarraAtributo::BarraAtributo(const sf::Font& fuente, std::string etiqueta,
                             sf::Vector2f posicion, sf::Vector2f tamano)
    : etiqueta_(std::move(etiqueta))
    , posicion_(posicion)
    , tamano_(tamano)
    , texto_(fuente, etiqueta_, 14)
    , valor_(fuente, "", 14)
{
    texto_.setFillColor(tema::kTextoSuave);
    valor_.setFillColor(tema::kTexto);

    marco_ = tema::paralelogramo(posicion_, tamano_);
    marco_.setFillColor(tema::kPanelBorde);
    marco_.setOutlineThickness(2.f);
    marco_.setOutlineColor(tema::kPanelBorde);

    hueco_ = tema::paralelogramo({ posicion_.x + 2.f, posicion_.y + 2.f },
                                 { tamano_.x - 4.f, tamano_.y - 4.f });
    hueco_.setFillColor(tema::kBarraFondo);

    for (float x = kPasoMuesca; x < tamano_.x - 4.f; x += kPasoMuesca)
    {
        sf::ConvexShape muesca = tema::paralelogramo(
            { posicion_.x + 2.f + x, posicion_.y + 2.f },
            { kAnchoMuesca, tamano_.y - 4.f });
        muesca.setFillColor(tema::conAlfa(tema::kPanelBorde, 190));
        muescas_.push_back(muesca);
    }

    establecerPosicion(posicion);
}

void BarraAtributo::establecerPosicion(sf::Vector2f posicion)
{
    posicion_ = posicion;
    rehacerFormas();

    texto_.setPosition({ posicion_.x + tema::kSesgo, posicion_.y - 18.f });

    const sf::FloatRect limites = valor_.getLocalBounds();
    valor_.setPosition({ posicion_.x + tamano_.x + tema::kSesgo - limites.size.x,
                         posicion_.y - 18.f });
}

void BarraAtributo::rehacerFormas()
{
    marco_ = tema::paralelogramo(posicion_, tamano_);
    marco_.setFillColor(tema::kPanelBorde);

    hueco_ = tema::paralelogramo({ posicion_.x + 2.f, posicion_.y + 2.f },
                                 { tamano_.x - 4.f, tamano_.y - 4.f });
    hueco_.setFillColor(tema::kBarraFondo);

    muescas_.clear();
    for (float x = kPasoMuesca; x < tamano_.x - 4.f; x += kPasoMuesca)
    {
        sf::ConvexShape muesca = tema::paralelogramo(
            { posicion_.x + 2.f + x, posicion_.y + 2.f },
            { kAnchoMuesca, tamano_.y - 4.f });
        muesca.setFillColor(tema::conAlfa(tema::kPanelBorde, 190));
        muescas_.push_back(muesca);
    }
}

void BarraAtributo::actualizar(const Atributo& atributo, float dt)
{
    nivel_ = atributo.porcentaje();

    if (rastro_ > nivel_) rastro_ = std::max(nivel_, rastro_ - kCaidaRastro * dt);
    else                  rastro_ = nivel_;

    const float util = tamano_.x - 4.f;

    estela_ = tema::paralelogramo({ posicion_.x + 2.f, posicion_.y + 2.f },
                                  { util * rastro_, tamano_.y - 4.f });
    estela_.setFillColor(tema::kBarraEstela);

    relleno_ = tema::paralelogramo({ posicion_.x + 2.f, posicion_.y + 2.f },
                                   { util * nivel_, tamano_.y - 4.f });
    relleno_.setFillColor(tema::segunNivel(nivel_));

    brillo_ = tema::paralelogramo({ posicion_.x + 2.f, posicion_.y + 2.f },
                                  { util * nivel_, (tamano_.y - 4.f) * 0.4f });
    brillo_.setFillColor(tema::kBarraBrillo);

    valor_.setString(util::aTexto(atributo.valor()));
    const sf::FloatRect limites = valor_.getLocalBounds();
    valor_.setPosition({ posicion_.x + tamano_.x + tema::kSesgo - limites.size.x,
                         posicion_.y - 18.f });
}

void BarraAtributo::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    objetivo.draw(marco_,   estados);
    objetivo.draw(hueco_,   estados);
    objetivo.draw(estela_,  estados);
    objetivo.draw(relleno_, estados);
    objetivo.draw(brillo_,  estados);

    for (const sf::ConvexShape& muesca : muescas_)
        objetivo.draw(muesca, estados);

    tema::dibujarConSombra(objetivo, texto_);
    tema::dibujarConSombra(objetivo, valor_);
}

}
