#include "Hud.hpp"

#include "Tema.hpp"
#include "Mascota.hpp"
#include "Utilidades.hpp"

namespace vp {

Hud::Hud(const sf::Font& fuente, sf::Vector2f origen, sf::Vector2f tamano)
    : titulo_        (fuente, "",         tema::kTextoTitulo)
    , subtitulo_     (fuente, "",         tema::kTextoChico)
    , estado_        (fuente, "",         tema::kTextoNormal)
    , tituloBitacora_(fuente, "BITACORA", tema::kTextoChico)
    , bitacora_      (fuente, "",         tema::kTextoChico)
{
    panel_.setPosition(origen);
    panel_.setSize(tamano);
    panel_.setFillColor(tema::kPanel);
    panel_.setOutlineThickness(1.f);
    panel_.setOutlineColor(tema::kPanelBorde);

    const float x = origen.x + 18.f;
    float       y = origen.y + 16.f;

    titulo_.setFillColor(tema::kTexto);
    titulo_.setPosition({ x, y });            y += 30.f;

    subtitulo_.setFillColor(tema::kTextoTenue);
    subtitulo_.setPosition({ x, y });         y += 22.f;

    estado_.setFillColor(tema::kAcento);
    estado_.setPosition({ x, y });            y += 34.f;

    const char* etiquetas[] = { "Saciedad", "Felicidad", "Energia", "Higiene", "Salud" };

    // Se deja hueco a la derecha de cada barra para el numero.
    const sf::Vector2f tamanoBarra(tamano.x - 36.f - 34.f, 20.f);

    barras_.reserve(5);
    for (const char* etiqueta : etiquetas)
    {
        barras_.emplace_back(fuente, etiqueta, sf::Vector2f(x, y), tamanoBarra);
        y += tamanoBarra.y + 10.f;
    }

    y += 14.f;
    tituloBitacora_.setFillColor(tema::kTextoTenue);
    tituloBitacora_.setPosition({ x, y });    y += 20.f;

    bitacora_.setFillColor(tema::kTextoSuave);
    bitacora_.setPosition({ x, y });
    bitacora_.setLineSpacing(1.35f);
}

void Hud::actualizar(const Mascota& mascota)
{
    barras_[0].actualizar(mascota.saciedad());
    barras_[1].actualizar(mascota.felicidad());
    barras_[2].actualizar(mascota.energia());
    barras_[3].actualizar(mascota.higiene());
    barras_[4].actualizar(mascota.salud());

    titulo_.setString(mascota.nombre());
    subtitulo_.setString(mascota.especie() + "  |  edad " +
                         util::formatearTiempo(mascota.edad()));
    estado_.setString(mascota.estado().nombre() + " - " + mascota.estado().mensaje());

    // Se muestran las ultimas lineas de la bitacora, de la mas nueva hacia atras.
    const auto& registro = mascota.bitacora();
    std::string texto;
    std::size_t mostradas = 0;

    for (auto it = registro.rbegin(); it != registro.rend() && mostradas < kLineasBitacora; ++it)
    {
        texto += "- " + *it + "\n";
        ++mostradas;
    }
    bitacora_.setString(texto);
}

void Hud::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    objetivo.draw(panel_,     estados);
    objetivo.draw(titulo_,    estados);
    objetivo.draw(subtitulo_, estados);
    objetivo.draw(estado_,    estados);

    for (const BarraAtributo& barra : barras_)
        objetivo.draw(barra, estados);

    objetivo.draw(tituloBitacora_, estados);
    objetivo.draw(bitacora_,       estados);
}

} // namespace vp
