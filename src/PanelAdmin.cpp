#include "PanelAdmin.hpp"

#include "Tema.hpp"
#include "VistaMascota.hpp"
#include "AdminMenu.hpp"
#include "Inventario.hpp"
#include "Mascota.hpp"
#include "Utilidades.hpp"

#include <cstddef>
#include <utility>

namespace vp {

PanelAdmin::PanelAdmin(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : titulo_        (fuente, "ADMIN_MENU",     tema::kTextoTitulo)
    , aviso_         (fuente, "Herramienta de desarrollo - F1 abre y cierra, ESC cierra",
                      tema::kTextoChico)
    , diagnostico_   (fuente, "",               tema::kTextoChico)
    , registro_      (fuente, "",               tema::kTextoChico)
    , seccionEstados_(fuente, "FORZAR ESTADO",  tema::kTextoChico)
    , seccionUtiles_ (fuente, "UTILIDADES",     tema::kTextoChico)
    , fuente_(fuente)
{
    origenPanel_ = { (tamanoVentana.x - kAncho) * 0.5f,
                     (tamanoVentana.y - kAlto)  * 0.5f };

    velo_.setSize(tamanoVentana);
    velo_.setFillColor(sf::Color(0, 0, 0, 170));

    panel_.setSize({ kAncho, kAlto });
    panel_.setPosition(origenPanel_);
    panel_.setFillColor(tema::kPanel);
    panel_.setOutlineThickness(2.f);
    panel_.setOutlineColor(tema::kAdmin);

    franja_.setSize({ kAncho, 4.f });
    franja_.setPosition(origenPanel_);
    franja_.setFillColor(tema::kAdmin);

    const float x = origenPanel_.x + 22.f;

    titulo_.setFillColor(tema::kAdmin);
    titulo_.setPosition({ x, origenPanel_.y + 18.f });

    aviso_.setFillColor(tema::kTextoTenue);
    aviso_.setPosition({ x, origenPanel_.y + 46.f });

    seccionEstados_.setFillColor(tema::kTextoSuave);
    seccionEstados_.setPosition({ x, origenPanel_.y + 76.f });

    seccionUtiles_.setFillColor(tema::kTextoSuave);
    seccionUtiles_.setPosition({ x, origenPanel_.y + 190.f });

    diagnostico_.setFillColor(tema::kTexto);
    diagnostico_.setPosition({ x, origenPanel_.y + 360.f });
    diagnostico_.setLineSpacing(1.35f);

    registro_.setFillColor(tema::kTextoTenue);
    registro_.setPosition({ x + 260.f, origenPanel_.y + 360.f });
    registro_.setLineSpacing(1.35f);
}

void PanelAdmin::enlazar(AdminMenu& admin, Mascota& mascota,
                         Inventario& inventario, VistaMascota& vista)
{
    botones_.clear();

    const sf::Vector2f tamanoChico(124.f, 30.f);
    const float        paso = tamanoChico.x + 10.f;

    float x = origenPanel_.x + 22.f;
    float y = origenPanel_.y + 98.f;
    int   columna = 0;

    for (TipoEstado tipo : todosLosEstados())
    {
        botones_.emplace_back(fuente_, nombreEstado(tipo), sf::Vector2f(x, y), tamanoChico,
                              [&admin, &mascota, tipo] { admin.forzarEstado(mascota, tipo); });
        botones_.back().establecerTamanoTexto(tema::kTextoChico);

        if (++columna % 4 == 0) { x = origenPanel_.x + 22.f; y += tamanoChico.y + 8.f; }
        else                    { x += paso; }
    }

    x = origenPanel_.x + 22.f;
    y = origenPanel_.y + 212.f;
    columna = 0;

    const auto agregar = [&](const std::string& etiqueta, Boton::Accion accion)
    {
        botones_.emplace_back(fuente_, etiqueta, sf::Vector2f(x, y), tamanoChico,
                              std::move(accion));
        botones_.back().establecerTamanoTexto(tema::kTextoChico);
        botones_.back().establecerColores(tema::kBotonPulsado, tema::kBotonHover);

        if (++columna % 4 == 0) { x = origenPanel_.x + 22.f; y += tamanoChico.y + 8.f; }
        else                    { x += paso; }
    };

    agregar("Rellenar todo", [&admin, &mascota] { admin.rellenarAtributos(mascota); });
    agregar("Vaciar todo",   [&admin, &mascota] { admin.vaciarAtributos(mascota); });
    agregar("Revivir",       [&admin, &mascota] { admin.revivir(mascota); });
    indiceInmortal_ = botones_.size();
    agregar("Inmortal",      [&admin]           { admin.alternarInmortalidad(); });
    agregar("Velocidad",     [&admin]           { admin.siguienteEscala(); });
    agregar("+1 dia",        [&admin, &mascota] { admin.envejecer(mascota, 1.f); });
    agregar("+100 monedas",  [&admin, &inventario] { admin.agregarMonedas(inventario, 100); });
    agregar("Modo figuras",  [&vista]           { vista.forzarProcedural(); });

    indiceMetrica_ = botones_.size();
    agregar("Metrica",       [&admin]           { admin.alternarMetrica(); });

    enlazado_ = true;
}

bool PanelAdmin::procesarClic(sf::Vector2f punto)
{
    for (Boton& boton : botones_)
        if (boton.procesarClic(punto)) return true;

    return panel_.getGlobalBounds().contains(punto);
}

void PanelAdmin::actualizar(sf::Vector2f raton, const AdminMenu& admin,
                            const Mascota& mascota, const VistaMascota& vista,
                            float fps)
{
    if (!enlazado_) return;

    for (Boton& boton : botones_)
        boton.actualizar(raton);

    const auto estados = todosLosEstados();
    for (std::size_t i = 0; i < estados.size() && i < botones_.size(); ++i)
        botones_[i].establecerActivo(estados[i] == mascota.tipoEstado());

    if (indiceInmortal_ < botones_.size())
        botones_[indiceInmortal_].establecerActivo(admin.inmortalidad());
    if (indiceMetrica_ < botones_.size())
        botones_[indiceMetrica_].establecerActivo(admin.metricaVisible());

    diagnostico_.setString(
        "Estado:      " + mascota.estado().nombre()                       + "\n" +
        "Actividad:   " + nombreActividad(mascota.actividad())            + "\n" +
        "Especie:     " + mascota.especie()                               + "\n" +
        "Genero:      " + nombreGenero(mascota.genero())                  + "\n" +
        "Viva:        " + std::string(mascota.estaViva() ? "si" : "no")   + "\n" +
        "Inmortal:    " + std::string(admin.inmortalidad() ? "si" : "no") + "\n" +
        "Velocidad:   x" + util::aTexto(admin.escalaTiempo())             + "\n" +
        "Render:      " + std::string(vista.nombreRender())               + "\n" +
        "FPS:         " + util::aTexto(fps));

    const auto& lineas = admin.registro();
    std::string texto  = "REGISTRO\n";
    std::size_t contadas = 0;

    for (auto it = lineas.rbegin(); it != lineas.rend() && contadas < 6; ++it, ++contadas)
        texto += "- " + *it + "\n";

    registro_.setString(texto);
}

void PanelAdmin::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    objetivo.draw(velo_,   estados);
    objetivo.draw(panel_,  estados);
    objetivo.draw(franja_, estados);

    objetivo.draw(titulo_,         estados);
    objetivo.draw(aviso_,          estados);
    objetivo.draw(seccionEstados_, estados);
    objetivo.draw(seccionUtiles_,  estados);

    for (const Boton& boton : botones_)
        objetivo.draw(boton, estados);

    objetivo.draw(diagnostico_, estados);
    objetivo.draw(registro_,    estados);
}

}
