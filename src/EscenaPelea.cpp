#include "EscenaPelea.hpp"

#include "Mascota.hpp"
#include "VistaMascota.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace vp {
namespace {

constexpr float kAnchoBarraVida = 300.f;
constexpr float kAltoBarraVida  = 22.f;
constexpr float kMargenBarra    = 40.f;
constexpr float kYBarra         = 96.f;

}

EscenaPelea::EscenaPelea(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , titulo_(fuente, "PELEA", tema::kTextoTitulo)
    , etiquetaJugador_(fuente, "TU", tema::kTextoChico)
    , etiquetaRival_(fuente, "RIVAL", tema::kTextoChico)
    , pie_(fuente, "FLECHAS MOVER   ESPACIO GOLPEAR   ESC HUIR", tema::kTextoChico)
    , anuncio_(fuente, "", 56)
{
    fondo_.setSize(tamanoVentana_);
    fondo_.setFillColor(tema::kFondo);

    barrido_ = tema::barridoCRT(tamanoVentana_);

    const sf::Vector2f tamanoMarco(320.f, 60.f);
    const sf::Vector2f posMarco((tamanoVentana_.x - tamanoMarco.x) / 2.f, 16.f);
    marquesina_ = tema::panelBiselado(posMarco, tamanoMarco, tema::kPanelBorde);

    titulo_.setFillColor(tema::kAcento);
    const sf::FloatRect lt = titulo_.getLocalBounds();
    titulo_.setOrigin({ lt.position.x + lt.size.x / 2.f, lt.position.y + lt.size.y / 2.f });
    titulo_.setPosition({ tamanoVentana_.x / 2.f, posMarco.y + tamanoMarco.y / 2.f });

    fondoVidaJugador_ = tema::paralelogramo({ kMargenBarra, kYBarra },
                                            { kAnchoBarraVida, kAltoBarraVida });
    fondoVidaJugador_.setFillColor(tema::kBarraFondo);

    const sf::Vector2f origenRival{ tamanoVentana_.x - kMargenBarra - kAnchoBarraVida, kYBarra };
    fondoVidaRival_ = tema::paralelogramo(origenRival, { kAnchoBarraVida, kAltoBarraVida });
    fondoVidaRival_.setFillColor(tema::kBarraFondo);

    etiquetaJugador_.setFillColor(tema::kTextoSuave);
    etiquetaJugador_.setPosition({ kMargenBarra, kYBarra - 22.f });

    etiquetaRival_.setFillColor(tema::kTextoSuave);
    const sf::FloatRect lr = etiquetaRival_.getLocalBounds();
    etiquetaRival_.setOrigin({ lr.position.x + lr.size.x, lr.position.y });
    etiquetaRival_.setPosition({ tamanoVentana_.x - kMargenBarra, kYBarra - 22.f });

    cuerpoRival_.setRadius(34.f);
    cuerpoRival_.setOrigin({ 34.f, 34.f });
    cuerpoRival_.setFillColor(tema::kMal);
    cuerpoRival_.setOutlineThickness(3.f);
    cuerpoRival_.setOutlineColor(tema::kPanelBorde);

    cabezaRival_.setRadius(18.f);
    cabezaRival_.setOrigin({ 18.f, 18.f });
    cabezaRival_.setFillColor(tema::kMal);
    cabezaRival_.setOutlineThickness(3.f);
    cabezaRival_.setOutlineColor(tema::kPanelBorde);

    const float altoTira = 34.f;
    tiraPie_ = tema::panelBiselado({ 0.f, tamanoVentana_.y - altoTira },
                                   { tamanoVentana_.x, altoTira });

    pie_.setFillColor(tema::kTextoSuave);
    const sf::FloatRect lp = pie_.getLocalBounds();
    pie_.setOrigin({ lp.position.x + lp.size.x / 2.f, 0.f });
    pie_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y - altoTira + 10.f });

    anuncio_.setFillColor(tema::kAcento);

    yPelea_ = tamanoVentana_.y * 0.58f;
}

void EscenaPelea::abrir(Mascota& mascota, VistaMascota& vista)
{
    mascota_ = &mascota;
    vista_   = &vista;

    fase_      = Fase::Presentacion;
    relojFase_ = 0.f;
    gano_      = false;

    vidaJugador_ = 100.f;
    vidaRival_   = 100.f;

    enfriamientoGolpeJugador_ = 0.f;
    enfriamientoGolpeRival_   = 0.f;
    avisoGolpeRival_          = 0.f;

    xJugador_ = kMargenArena + 60.f;
    xRival_   = tamanoVentana_.x - kMargenArena - 60.f;

    vista_->establecerPosicion({ xJugador_, yPelea_ });
    vista_->establecerEscala(2.6f);

    cuerpoRival_.setPosition({ xRival_, yPelea_ });
    cabezaRival_.setPosition({ xRival_, yPelea_ - 46.f });
    cuerpoRival_.setFillColor(tema::kMal);
    cabezaRival_.setFillColor(tema::kMal);

    refrescarBarras();
    anunciar("¡PELEA!", tema::kAcento);
}

void EscenaPelea::manejarEvento(const sf::Event& evento)
{
    if (fase_ != Fase::Combate) return;

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {
        using Tecla = sf::Keyboard::Key;
        switch (tecla->code)
        {
            case Tecla::Escape:
                terminarCombate(false);
                break;

            case Tecla::Space:
            case Tecla::J:
                golpearSiEnRango();
                break;

            default: break;
        }
    }
}

void EscenaPelea::actualizar(float dt)
{
    if (fase_ == Fase::Cerrada) return;

    relojFase_ += dt;

    if (anuncioRestante_ > 0.f)
    {
        anuncioRestante_ -= dt;

        const float t = anuncioRestante_ / 1.4f;
        const auto  alfa = static_cast<std::uint8_t>(255.f * std::min(1.f, t * 3.f));
        sf::Color   color = anuncio_.getFillColor();
        color.a = alfa;
        anuncio_.setFillColor(color);
    }

    switch (fase_)
    {
        case Fase::Presentacion:
            if (relojFase_ >= kDuracionPresentacion)
            {
                fase_      = Fase::Combate;
                relojFase_ = 0.f;
            }
            break;

        case Fase::Combate:
            actualizarCombate(dt);
            break;

        case Fase::Resultado:
            if (relojFase_ >= kDuracionResultado)
                fase_ = Fase::Cerrada;
            break;

        case Fase::Cerrada:
            break;
    }
}

void EscenaPelea::actualizarCombate(float dt)
{
    if (enfriamientoGolpeJugador_ > 0.f) enfriamientoGolpeJugador_ -= dt;

    const float minX = kMargenArena;
    const float maxX = tamanoVentana_.x - kMargenArena;

    float direccion = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        direccion -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        direccion += 1.f;

    xJugador_ = std::clamp(xJugador_ + direccion * kVelocidadJugador * dt, minX, maxX);
    vista_->establecerPosicion({ xJugador_, yPelea_ });

    actualizarIaRival(dt);

    cuerpoRival_.setPosition({ xRival_, yPelea_ });
    cabezaRival_.setPosition({ xRival_, yPelea_ - 46.f });

    const sf::Color colorRival = (avisoGolpeRival_ > 0.f) ? tema::kAcento : tema::kMal;
    cuerpoRival_.setFillColor(colorRival);
    cabezaRival_.setFillColor(colorRival);

    refrescarBarras();

    if (vidaJugador_ <= 0.f)      terminarCombate(false);
    else if (vidaRival_ <= 0.f)   terminarCombate(true);
}

void EscenaPelea::actualizarIaRival(float dt)
{
    const float distancia    = xRival_ - xJugador_;
    const float distanciaAbs = std::fabs(distancia);

    if (avisoGolpeRival_ > 0.f)
    {
        avisoGolpeRival_ -= dt;
        if (avisoGolpeRival_ <= 0.f)
        {
            if (distanciaAbs <= kRangoGolpe)
                vidaJugador_ = std::max(0.f, vidaJugador_ - kDanoGolpeRival);

            enfriamientoGolpeRival_ = kEnfriamientoRival;
        }
        return;
    }

    if (enfriamientoGolpeRival_ > 0.f) enfriamientoGolpeRival_ -= dt;

    if (distanciaAbs <= kRangoGolpe)
    {
        if (enfriamientoGolpeRival_ <= 0.f) avisoGolpeRival_ = kAvisoRival;
        return;
    }

    const float minX = kMargenArena;
    const float maxX = tamanoVentana_.x - kMargenArena;
    const float paso = (distancia > 0.f ? -1.f : 1.f) * kVelocidadRival * dt;
    xRival_ = std::clamp(xRival_ + paso, minX, maxX);
}

void EscenaPelea::golpearSiEnRango()
{
    if (enfriamientoGolpeJugador_ > 0.f) return;

    enfriamientoGolpeJugador_ = kEnfriamientoJugador;

    if (std::fabs(xRival_ - xJugador_) <= kRangoGolpe)
        vidaRival_ = std::max(0.f, vidaRival_ - kDanoGolpeJugador);
}

void EscenaPelea::terminarCombate(bool gano)
{
    gano_      = gano;
    fase_      = Fase::Resultado;
    relojFase_ = 0.f;

    if (!mascota_) return;

    if (gano)
    {
        mascota_->agregarMonedas(kMonedasPorVictoria);
        mascota_->felicidad().modificar(kFelicidadPorVictoria);
        mascota_->energia().modificar(-kEnergiaPorPelea);
        anunciar("GANASTE +" + std::to_string(kMonedasPorVictoria) + " MONEDAS", tema::kBien);
    }
    else
    {
        mascota_->energia().modificar(-(kEnergiaPorPelea + kEnergiaExtraDerrota));
        anunciar("PERDISTE", tema::kMal);
    }
}

void EscenaPelea::anunciar(const std::string& texto, sf::Color color)
{
    anuncio_.setString(texto);
    anuncio_.setFillColor(color);

    const sf::FloatRect limites = anuncio_.getLocalBounds();
    anuncio_.setOrigin({ limites.position.x + limites.size.x / 2.f,
                         limites.position.y + limites.size.y / 2.f });
    anuncio_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y * 0.35f });
    anuncioRestante_ = 1.4f;
}

void EscenaPelea::refrescarBarras()
{
    const float fJugador = std::clamp(vidaJugador_ / 100.f, 0.f, 1.f);
    const float fRival   = std::clamp(vidaRival_ / 100.f, 0.f, 1.f);

    tema::ponerParalelogramo(rellenoVidaJugador_, { kMargenBarra, kYBarra },
                             { kAnchoBarraVida * fJugador, kAltoBarraVida });
    rellenoVidaJugador_.setFillColor(tema::segunNivel(fJugador));

    const sf::Vector2f origenRival{ tamanoVentana_.x - kMargenBarra - kAnchoBarraVida, kYBarra };
    tema::ponerParalelogramo(rellenoVidaRival_, origenRival,
                             { kAnchoBarraVida * fRival, kAltoBarraVida });
    rellenoVidaRival_.setFillColor(tema::segunNivel(fRival));
}

void EscenaPelea::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    if (fase_ == Fase::Cerrada) return;

    objetivo.draw(fondo_, estados);

    if (vista_) objetivo.draw(*vista_, estados);
    objetivo.draw(cuerpoRival_, estados);
    objetivo.draw(cabezaRival_, estados);

    marquesina_.dibujar(objetivo);
    tema::dibujarConSombra(objetivo, titulo_, 3.f);

    objetivo.draw(fondoVidaJugador_, estados);
    objetivo.draw(rellenoVidaJugador_, estados);
    objetivo.draw(fondoVidaRival_, estados);
    objetivo.draw(rellenoVidaRival_, estados);
    tema::dibujarConSombra(objetivo, etiquetaJugador_);
    tema::dibujarConSombra(objetivo, etiquetaRival_);

    if (anuncioRestante_ > 0.f) objetivo.draw(anuncio_, estados);

    tiraPie_.dibujar(objetivo);
    objetivo.draw(pie_, estados);

    objetivo.draw(barrido_, estados);
}

}
