#include "PantallaMenu.hpp"

#include "Mosaico.hpp"
#include "Tema.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace vp {

namespace {

constexpr unsigned kTamTitulo  = 56;
constexpr unsigned kTamOpcion  = 40;
constexpr float    kSeparacion = 30.f;

constexpr float kGananciaAudio = 6.f;
constexpr float kAtaqueAudio   = 18.f;
constexpr float kCaidaAudio    = 4.f;

}

PantallaMenu::PantallaMenu(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , titulo_(fuente, "VIRTUALPET", kTamTitulo)
    , pie_(fuente, "FLECHAS MOVER    ENTER ELEGIR    M MUSICA", tema::kTextoChico)
{
    construirFondo();

    const sf::Vector2f tamanoMarco(600.f, 96.f);
    const sf::Vector2f posMarco((tamanoVentana_.x - tamanoMarco.x) / 2.f, 46.f);
    marquesina_ = tema::panelBiselado(posMarco, tamanoMarco, tema::kPanelBorde);

    titulo_.setFillColor(tema::kAcento);
    const sf::FloatRect limites = titulo_.getLocalBounds();
    titulo_.setOrigin({ limites.position.x + limites.size.x / 2.f,
                        limites.position.y + limites.size.y / 2.f });
    titulo_.setPosition({ tamanoVentana_.x / 2.f, posMarco.y + tamanoMarco.y / 2.f });
    titulo_.setRotation(sf::degrees(-8.f));

    const std::vector<std::string> opciones { "Jugar", "Opciones", "Salir" };
    textosMenu_.reserve(opciones.size());
    placas_.reserve(opciones.size());

    for (const std::string& texto : opciones)
    {
        textosMenu_.emplace_back(fuente_, texto, kTamOpcion);
        placas_.emplace_back();
    }

    colocarOpciones();

    senalador_ = tema::senaladorMenu(tema::kAcento);

    const float altoTira = 34.f;
    tiraPie_ = tema::panelBiselado({ 0.f, tamanoVentana_.y - altoTira },
                                   { tamanoVentana_.x, altoTira });

    pie_.setFillColor(tema::kTextoSuave);
    const sf::FloatRect limitesPie = pie_.getLocalBounds();
    pie_.setOrigin({ limitesPie.position.x + limitesPie.size.x / 2.f, 0.f });
    pie_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y - altoTira + 10.f });

    barrido_ = tema::barridoCRT(tamanoVentana_);

    if (musica_.load("assets/sound/Menu.ogg"))
    {
        musica_.setLoop(true);
        musica_.setVolume(45.f);
        musica_.play();
    }

    if (pistaAnalisis_.openFromFile("assets/sound/Menu.ogg"))
    {

        analisisDisponible_   = true;
        tasaMuestreoAnalisis_ = pistaAnalisis_.getSampleRate();
        canalesAnalisis_      = pistaAnalisis_.getChannelCount();
        bufferAnalisis_.resize(static_cast<std::size_t>(tasaMuestreoAnalisis_) * canalesAnalisis_);
    }

    botonMusica_.cargar("assets/images/speaker.png", { 26.f, 22.f }, 0.03f);
    botonMusica_.establecerActivo(musica_.isPlaying());
}

void PantallaMenu::construirFondo()
{

    const sf::Vector2f tamanoLienzo(tamanoVentana_.x + kMargenFondo * 2.f,
                                    tamanoVentana_.y + kMargenFondo * 2.f);

    if (!componerMosaico(lienzoFondo_, tamanoLienzo,
                         { "assets/images/conejo_macho.txt",
                           "assets/images/castor_macho.txt" },
                         tema::kFondoMosaico))
        return;

    fondo_.emplace(lienzoFondo_.getTexture());
    fondo_->setOrigin({ tamanoLienzo.x / 2.f, tamanoLienzo.y / 2.f });
    fondo_->setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y / 2.f });

    velo_.setSize(tamanoVentana_);
    velo_.setFillColor(sf::Color(20, 12, 30, 90));
}

void PantallaMenu::colocarOpciones()
{
    const float altoTotal = placas_.size() * kAltoPlaca +
                            (placas_.size() - 1) * kHuecoPlaca;
    const float inicioY   = (tamanoVentana_.y - altoTotal) / 2.f + 24.f;
    const float x         = (tamanoVentana_.x - kAnchoPlaca) / 2.f;

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        const float y = inicioY + i * (kAltoPlaca + kHuecoPlaca);

        placas_[i] = tema::paralelogramo({ x, y }, { kAnchoPlaca, kAltoPlaca });

        sf::Text& texto = textosMenu_[i];
        const sf::FloatRect limites = texto.getLocalBounds();
        texto.setOrigin({ limites.position.x + limites.size.x / 2.f,
                          limites.position.y + limites.size.y / 2.f });
        texto.setPosition({ tamanoVentana_.x / 2.f, y + kAltoPlaca / 2.f });
    }
}

void PantallaMenu::mover(int direccion)
{
    const int total = static_cast<int>(textosMenu_.size());
    seleccion_ = static_cast<std::size_t>((static_cast<int>(seleccion_) + direccion + total) % total);
}

void PantallaMenu::elegir(std::size_t indice)
{
    switch (static_cast<OpcionMenu>(indice))
    {
        case OpcionMenu::Jugar:    solicitar(Transicion::Seleccion); break;
        case OpcionMenu::Opciones: solicitar(Transicion::Opciones);  break;
        case OpcionMenu::Salir:    solicitar(Transicion::Salir);     break;
    }
}

void PantallaMenu::manejarEvento(const sf::Event& evento)
{
    if (evento.is<sf::Event::Closed>())
    {
        solicitar(Transicion::Salir);
        return;
    }

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {
        using Tecla = sf::Keyboard::Key;
        switch (tecla->code)
        {
            case Tecla::Up:
            case Tecla::W:      mover(-1); break;

            case Tecla::Down:
            case Tecla::S:      mover(+1); break;

            case Tecla::Enter:
            case Tecla::Space:  elegir(seleccion_); break;

            case Tecla::Escape: solicitar(Transicion::Salir); break;

            case Tecla::M:
            {

                const bool sonando = musica_.isPlaying();
                if (sonando) musica_.pause(); else musica_.play();
                botonMusica_.establecerActivo(!sonando);
                break;
            }

            default: break;
        }
        return;
    }

    if (const auto* movimiento = evento.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2f punto(movimiento->position);

        const bool seMovio = (punto != raton_);
        raton_ = punto;
        if (!seMovio) return;

        for (std::size_t i = 0; i < placas_.size(); ++i)
            if (placas_[i].getGlobalBounds().contains(raton_))
                seleccion_ = i;
        return;
    }

    if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
    {
        if (clic->button != sf::Mouse::Button::Left) return;

        const sf::Vector2f punto(clic->position);

        if (botonMusica_.contiene(punto))
        {
            const bool sonando = musica_.isPlaying();
            if (sonando) musica_.pause(); else musica_.play();
            botonMusica_.establecerActivo(!sonando);
            return;
        }

        for (std::size_t i = 0; i < placas_.size(); ++i)
        {
            if (placas_[i].getGlobalBounds().contains(punto))
            {
                elegir(i);
                return;
            }
        }
    }
}

void PantallaMenu::actualizarNivelAudio(float dt)
{
    if (!analisisDisponible_ || !musica_.isPlaying())
    {
        nivelAudio_ += (0.f - nivelAudio_) * std::min(1.f, dt * kCaidaAudio);
        return;
    }

    std::uint64_t aLeer = static_cast<std::uint64_t>(
        dt * static_cast<float>(tasaMuestreoAnalisis_ * canalesAnalisis_));
    if (aLeer == 0) return;
    aLeer = std::min<std::uint64_t>(aLeer, bufferAnalisis_.size());

    std::uint64_t leidas = pistaAnalisis_.read(bufferAnalisis_.data(), aLeer);
    if (leidas == 0)
    {

        pistaAnalisis_.seek(std::uint64_t{ 0 });
        leidas = pistaAnalisis_.read(bufferAnalisis_.data(), aLeer);
    }
    if (leidas == 0) return;

    double sumaCuadrados = 0.0;
    for (std::uint64_t i = 0; i < leidas; ++i)
    {
        const double muestra = bufferAnalisis_[i] / 32768.0;
        sumaCuadrados += muestra * muestra;
    }
    const float rms = static_cast<float>(std::sqrt(sumaCuadrados / static_cast<double>(leidas)));

    const float objetivo  = std::min(1.f, rms * kGananciaAudio);
    const float velocidad = (objetivo > nivelAudio_) ? kAtaqueAudio : kCaidaAudio;
    nivelAudio_ += (objetivo - nivelAudio_) * std::min(1.f, dt * velocidad);
}

void PantallaMenu::actualizar(float dt)
{
    reloj_ += dt;
    actualizarNivelAudio(dt);

    if (fondo_)
    {
        const float derivaX = std::sin(reloj_ * 0.35f) * (kMargenFondo * 0.55f);
        const float derivaY = std::cos(reloj_ * 0.27f) * (kMargenFondo * 0.35f);
        const float pulso   = 1.f + nivelAudio_ * 0.05f;

        fondo_->setPosition({ tamanoVentana_.x / 2.f + derivaX,
                             tamanoVentana_.y / 2.f + derivaY });
        fondo_->setScale({ pulso, pulso });
    }

    velo_.setFillColor(sf::Color(20, 12, 30,
        static_cast<std::uint8_t>(70.f + nivelAudio_ * 45.f)));

    const float altoTotal = placas_.size() * kAltoPlaca +
                            (placas_.size() - 1) * kHuecoPlaca;
    const float inicioY   = (tamanoVentana_.y - altoTotal) / 2.f + 24.f;
    const float x         = (tamanoVentana_.x - kAnchoPlaca) / 2.f;

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        const bool elegida = (i == seleccion_);

        placas_[i].setFillColor(elegida ? tema::kAcento : tema::kBoton);
        placas_[i].setOutlineThickness(2.f);
        placas_[i].setOutlineColor(elegida ? tema::kTexto : tema::kPanelBorde);

        textosMenu_[i].setFillColor(elegida ? tema::kFondo : tema::kTexto);

        const float escala = elegida ? 1.f + std::sin(reloj_ * 4.f) * 0.03f : 1.f;
        textosMenu_[i].setScale({ escala, escala });
    }

    const float y = inicioY + seleccion_ * (kAltoPlaca + kHuecoPlaca);
    const float empuje = std::sin(reloj_ * 5.f) * 4.f;
    senalador_.setPosition({ x - 34.f + empuje, y + kAltoPlaca / 2.f - 9.f });
}

void PantallaMenu::dibujar(sf::RenderTarget& objetivo) const
{
    if (fondo_) objetivo.draw(*fondo_);
    objetivo.draw(velo_);

    marquesina_.dibujar(objetivo);
    tema::dibujarConSombra(objetivo, titulo_, 3.f);

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        objetivo.draw(placas_[i]);
        tema::dibujarConSombra(objetivo, textosMenu_[i]);
    }

    objetivo.draw(senalador_);

    tiraPie_.dibujar(objetivo);
    objetivo.draw(pie_);

    objetivo.draw(botonMusica_);

    objetivo.draw(barrido_);
}

}
