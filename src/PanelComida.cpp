#include "PanelComida.hpp"

#include "Inventario.hpp"
#include "Objetos.hpp"
#include "Utilidades.hpp"

#include <utility>

namespace vp {
namespace {

constexpr float kAltoFila = 34.f;
constexpr float kHueco    = 6.f;

} // namespace sin nombre

PanelComida::PanelComida(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , titulo_(fuente, "DESPENSA", tema::kTextoTitulo)
    , aviso_ (fuente, "Teclas 1-4 para elegir   -   ESC cierra", tema::kTextoChico)
{
    origen_ = { (tamanoVentana.x - kAncho) * 0.5f,
                (tamanoVentana.y - kAlto)  * 0.5f };

    velo_.setSize(tamanoVentana);
    velo_.setFillColor(sf::Color(0, 0, 0, 150));

    panel_ = tema::panelBiselado(origen_, { kAncho, kAlto });

    titulo_.setFillColor(tema::kAcento);
    titulo_.setPosition({ origen_.x + 20.f, origen_.y + 14.f });

    aviso_.setFillColor(tema::kTextoTenue);
    aviso_.setPosition({ origen_.x + 20.f, origen_.y + kAlto - 26.f });
}

void PanelComida::refrescar(const Inventario& inventario, AlElegir alElegir)
{
    botones_.clear();
    detalles_.clear();

    const auto indices = inventario.indicesPorCategoria("Comida");

    float y = origen_.y + 48.f;
    int   tecla = 1;

    for (std::size_t indice : indices)
    {
        const Objeto* objeto = inventario.objeto(indice);
        if (!objeto || objeto->agotado()) continue;

        botones_.emplace_back(fuente_, objeto->nombre(),
                              sf::Vector2f(origen_.x + 20.f, y),
                              sf::Vector2f(200.f, kAltoFila),
                              [alElegir, indice] { if (alElegir) alElegir(indice); });
        botones_.back().establecerTamanoTexto(tema::kTextoChico);
        botones_.back().establecerTecla(std::to_string(tecla++));

        // A la derecha, lo que alimenta y cuantas raciones quedan.
        std::string texto = objeto->detalle();
        if (!objeto->esInfinito())
            texto += "   x" + std::to_string(objeto->usosRestantes());

        detalles_.emplace_back(fuente_, texto, tema::kTextoChico);
        detalles_.back().setFillColor(tema::kTextoSuave);
        detalles_.back().setPosition({ origen_.x + 232.f, y + 9.f });

        y += kAltoFila + kHueco;
    }

    if (botones_.empty())
    {
        // Sin comida no hay nada que elegir: se dice, en vez de dejar el panel
        // vacio y que parezca que esta roto.
        detalles_.emplace_back(fuente_, "No queda nada en la despensa.", tema::kTextoNormal);
        detalles_.back().setFillColor(tema::kMedio);
        detalles_.back().setPosition({ origen_.x + 20.f, origen_.y + 56.f });
    }
}

void PanelComida::actualizar(sf::Vector2f raton)
{
    if (!visible_) return;

    for (Boton& boton : botones_)
        boton.actualizar(raton);
}

bool PanelComida::procesarClic(sf::Vector2f punto)
{
    if (!visible_) return false;

    for (Boton& boton : botones_)
    {
        if (boton.procesarClic(punto))
        {
            cerrar();
            return true;
        }
    }

    // Un clic fuera del panel lo cierra; dentro, se lo traga para que no llegue
    // a la botonera que hay debajo.
    if (!panel_.cara.getGlobalBounds().contains(punto)) cerrar();
    return true;
}

bool PanelComida::elegirPorIndice(std::size_t indice)
{
    if (!visible_ || indice >= botones_.size()) return false;

    botones_[indice].accionar();
    cerrar();
    return true;
}

void PanelComida::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    if (!visible_) return;

    objetivo.draw(velo_, estados);
    panel_.dibujar(objetivo);

    tema::dibujarConSombra(objetivo, titulo_);

    for (const Boton& boton : botones_)
        objetivo.draw(boton, estados);

    for (const sf::Text& texto : detalles_)
        objetivo.draw(texto, estados);

    objetivo.draw(aviso_, estados);
}

} // namespace vp
