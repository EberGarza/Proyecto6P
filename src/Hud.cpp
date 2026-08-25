#include "Hud.hpp"

#include "Mascota.hpp"
#include "Utilidades.hpp"

namespace vp {
namespace {

constexpr float kMargen      = 20.f;
constexpr float kAltoSalud   = 20.f;
constexpr float kAltoMenor   = 14.f;
constexpr float kHuecoMenor  = 14.f;

}

Hud::Hud(const sf::Font& fuente, sf::Vector2f tamanoVentana, float alturaTira)
    : tamanoVentana_(tamanoVentana)
    , nombre_  (fuente, "", 24)
    , especie_ (fuente, "", 13)
    , estado_  (fuente, "", 20)
    , bitacora_(fuente, "", 12)
{
    const float ancho = tamanoVentana_.x - kMargen * 2.f;

    tira_ = tema::panelBiselado({ 0.f, 0.f }, { tamanoVentana_.x, alturaTira });

    chapaNombre_ = tema::panelBiselado({ kMargen, 8.f }, { 320.f, 30.f },
                                       tema::kPanelBorde);

    nombre_.setFillColor(tema::kAcento);
    nombre_.setPosition({ kMargen + 10.f, 8.f });

    especie_.setFillColor(tema::kTextoSuave);
    especie_.setPosition({ kMargen + 340.f, 18.f });

    estado_.setFillColor(tema::kTexto);

    barras_.reserve(5);
    barras_.emplace_back(fuente, "SALUD", sf::Vector2f{ kMargen, 62.f },
                         sf::Vector2f{ ancho, kAltoSalud });

    const char* menores[] = { "SACIEDAD", "FELICIDAD", "ENERGIA", "HIGIENE" };
    const float anchoMenor = (ancho - kHuecoMenor * 3.f) / 4.f;

    for (int i = 0; i < 4; ++i)
    {
        barras_.emplace_back(fuente, menores[i],
                             sf::Vector2f{ kMargen + i * (anchoMenor + kHuecoMenor), 108.f },
                             sf::Vector2f{ anchoMenor, kAltoMenor });
    }

    const float altoTeletipo = 74.f;
    fondoBitacora_.setSize({ 340.f, altoTeletipo });
    fondoBitacora_.setPosition({ kMargen, tamanoVentana_.y - 118.f - altoTeletipo - 6.f });
    fondoBitacora_.setFillColor(sf::Color(12, 8, 20, 170));

    bitacora_.setFillColor(tema::kTextoSuave);
    bitacora_.setLineSpacing(1.3f);
    bitacora_.setPosition({ fondoBitacora_.getPosition().x + 8.f,
                            fondoBitacora_.getPosition().y + 6.f });
}

void Hud::actualizar(const Mascota& mascota, float dt)
{
    barras_[0].actualizar(mascota.salud(), dt);
    barras_[1].actualizar(mascota.saciedad(), dt);
    barras_[2].actualizar(mascota.felicidad(), dt);
    barras_[3].actualizar(mascota.energia(), dt);
    barras_[4].actualizar(mascota.higiene(), dt);

    nombre_.setString(mascota.nombre());
    especie_.setString(mascota.especie() + "   " + util::formatearTiempo(mascota.edad()));

    estado_.setString(mascota.estado().nombre());
    estado_.setFillColor(mascota.estaViva() ? tema::kAcento : tema::kMal);
    sf::FloatRect limites = estado_.getLocalBounds();
    estado_.setPosition({ tamanoVentana_.x - kMargen - limites.size.x - 10.f, 8.f });

    const auto& registro = mascota.bitacora();
    std::string texto;
    std::size_t mostradas = 0;

    for (auto it = registro.rbegin(); it != registro.rend() && mostradas < kLineasBitacora; ++it)
    {
        texto += "> " + *it + "\n";
        ++mostradas;
    }
    bitacora_.setString(texto);
}

void Hud::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    tira_.dibujar(objetivo);
    chapaNombre_.dibujar(objetivo);

    tema::dibujarConSombra(objetivo, nombre_);
    tema::dibujarConSombra(objetivo, especie_);
    tema::dibujarConSombra(objetivo, estado_);

    for (const BarraAtributo& barra : barras_)
        objetivo.draw(barra, estados);

    objetivo.draw(fondoBitacora_, estados);
    objetivo.draw(bitacora_, estados);
}

}
