#include "PantallaOpciones.hpp"

#include "GestorGuardado.hpp"
#include "Mosaico.hpp"

#include <cmath>
#include <filesystem>
#include <string>

namespace vp {

namespace {

const sf::Color kAmarillo(255, 220, 80);
const sf::Color kFondoBase(38, 26, 54);

constexpr unsigned kTamTitulo = 44;
constexpr unsigned kTamFila   = 22;

constexpr float kAnchoBarra = 150.f;
constexpr float kAltoBarra  = 14.f;

}

PantallaOpciones::PantallaOpciones(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , titulo_(fuente, "OPCIONES", kTamTitulo)
    , pie_(fuente, "ARRIBA/ABAJO ELEGIR    IZQ/DER AJUSTAR    ESC VOLVER",
           tema::kTextoChico)
{

    if (componerMosaico(lienzoFondo_, tamanoVentana_,
                        { "assets/images/conejo_hembra.txt",
                          "assets/images/castor_hembra.txt" },
                        kFondoBase))
        fondo_.emplace(lienzoFondo_.getTexture());

    velo_.setSize(tamanoVentana_);
    velo_.setFillColor(sf::Color(20, 12, 30, 130));

    construirInterfaz();

    if (musica_.load("assets/sound/MenuOpciones.ogg"))
    {
        musica_.setLoop(true);
        musica_.setVolume(static_cast<float>(volumen_));
        musica_.play();
    }

    refrescarTextos();
}

void PantallaOpciones::construirInterfaz()
{

    const sf::Vector2f tamanoMarco(420.f, 84.f);
    const sf::Vector2f posMarco((tamanoVentana_.x - tamanoMarco.x) / 2.f, 52.f);
    marquesina_ = tema::panelBiselado(posMarco, tamanoMarco, tema::kPanelBorde);

    titulo_.setFillColor(kAmarillo);
    const sf::FloatRect lt = titulo_.getLocalBounds();
    titulo_.setOrigin({ lt.position.x + lt.size.x / 2.f,
                        lt.position.y + lt.size.y / 2.f });
    titulo_.setPosition({ tamanoVentana_.x / 2.f, posMarco.y + tamanoMarco.y / 2.f });

    const auto  total     = static_cast<std::size_t>(Fila::Total);
    const float altoTotal = total * kAltoPlaca + (total - 1) * kHuecoPlaca;
    const float inicioY   = (tamanoVentana_.y - altoTotal) / 2.f + 30.f;
    const float x         = (tamanoVentana_.x - kAnchoPlaca) / 2.f;

    placas_.reserve(total);
    etiquetas_.reserve(total);
    valores_.reserve(total);

    for (std::size_t i = 0; i < total; ++i)
    {
        const float y = inicioY + i * (kAltoPlaca + kHuecoPlaca);

        placas_.push_back(tema::paralelogramo({ x, y }, { kAnchoPlaca, kAltoPlaca }));

        etiquetas_.emplace_back(fuente_, "", kTamFila);
        etiquetas_.back().setPosition({ x + 24.f, y + 12.f });

        valores_.emplace_back(fuente_, "", kTamFila);
        valores_.back().setPosition({ x + 250.f, y + 12.f });
    }

    const float yVolumen = inicioY + static_cast<float>(Fila::Volumen) *
                                     (kAltoPlaca + kHuecoPlaca);
    origenVolumen_ = { x + kAnchoPlaca - 28.f - 52.f - kAnchoBarra,
                       yVolumen + (kAltoPlaca - kAltoBarra) / 2.f };
    anchoVolumen_  = kAnchoBarra;

    fondoVolumen_ = tema::paralelogramo(origenVolumen_, { kAnchoBarra, kAltoBarra }, 5.f);
    fondoVolumen_.setFillColor(tema::kBarraFondo);
    fondoVolumen_.setOutlineThickness(1.f);
    fondoVolumen_.setOutlineColor(tema::kPanelBorde);

    senalador_.setPointCount(3);
    senalador_.setPoint(0, { 0.f,  0.f });
    senalador_.setPoint(1, { 14.f, 8.f });
    senalador_.setPoint(2, { 0.f, 16.f });
    senalador_.setFillColor(kAmarillo);

    const float altoTira = 34.f;
    tiraPie_ = tema::panelBiselado({ 0.f, tamanoVentana_.y - altoTira },
                                   { tamanoVentana_.x, altoTira });

    pie_.setFillColor(tema::kTextoSuave);
    const sf::FloatRect lp = pie_.getLocalBounds();
    pie_.setOrigin({ lp.position.x + lp.size.x / 2.f, 0.f });
    pie_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y - altoTira + 10.f });

    barrido_ = tema::barridoCRT(tamanoVentana_);
}

void PantallaOpciones::refrescarTextos()
{
    const std::string etiquetas[] = { "Musica", "Volumen", "Partida guardada", "Volver" };
    const std::string valores[]   = {
        musicaActiva_ ? "activada" : "silenciada",
        std::to_string(volumen_),
        borrada_ ? "borrada" : "borrar",
        ""
    };

    const float derecha = (tamanoVentana_.x - kAnchoPlaca) / 2.f + kAnchoPlaca - 28.f;

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        etiquetas_[i].setString(etiquetas[i]);
        valores_[i].setString(valores[i]);

        sf::Text& valor = valores_[i];
        const sf::FloatRect lv = valor.getLocalBounds();
        valor.setOrigin({ lv.position.x + lv.size.x, lv.position.y });
        valor.setPosition({ derecha, valor.getPosition().y });
    }

    const float fraccion = static_cast<float>(volumen_) / 100.f;
    barraVolumen_ = tema::paralelogramo(origenVolumen_,
                                        { anchoVolumen_ * fraccion, kAltoBarra }, 5.f);
    barraVolumen_.setFillColor(musicaActiva_ ? kAmarillo : tema::kTextoTenue);
}

void PantallaOpciones::ajustar(int direccion)
{
    switch (static_cast<Fila>(seleccion_))
    {
        case Fila::Musica:
            musicaActiva_ = !musicaActiva_;
            if (musicaActiva_) musica_.play(); else musica_.pause();
            break;

        case Fila::Volumen:
            volumen_ += direccion * 5;
            if (volumen_ < 0)   volumen_ = 0;
            if (volumen_ > 100) volumen_ = 100;
            musica_.setVolume(static_cast<float>(volumen_));
            break;

        default:
            break;
    }
    refrescarTextos();
}

void PantallaOpciones::activar()
{
    switch (static_cast<Fila>(seleccion_))
    {
        case Fila::Musica:
        case Fila::Volumen:
            ajustar(+1);
            break;

        case Fila::BorrarPartida:
        {
            std::error_code error;
            std::filesystem::remove(GestorGuardado::rutaPorDefecto(), error);
            borrada_ = true;
            refrescarTextos();
            break;
        }

        case Fila::Volver:
            solicitar(Transicion::Menu);
            break;

        default:
            break;
    }
}

void PantallaOpciones::manejarEvento(const sf::Event& evento)
{
    if (evento.is<sf::Event::Closed>())
    {
        solicitar(Transicion::Salir);
        return;
    }

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {
        using Tecla = sf::Keyboard::Key;
        const int total = static_cast<int>(placas_.size());

        switch (tecla->code)
        {
            case Tecla::Up:
            case Tecla::W:
                seleccion_ = static_cast<std::size_t>((static_cast<int>(seleccion_) - 1 + total) % total);
                break;

            case Tecla::Down:
            case Tecla::S:
                seleccion_ = static_cast<std::size_t>((static_cast<int>(seleccion_) + 1) % total);
                break;

            case Tecla::Left:
            case Tecla::A:      ajustar(-1); break;

            case Tecla::Right:
            case Tecla::D:      ajustar(+1); break;

            case Tecla::Enter:
            case Tecla::Space:  activar(); break;

            case Tecla::Escape: solicitar(Transicion::Menu); break;

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
            if (placas_[i].getGlobalBounds().contains(punto))
                seleccion_ = i;
        return;
    }

    if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
    {
        if (clic->button != sf::Mouse::Button::Left) return;

        const sf::Vector2f punto(clic->position);
        for (std::size_t i = 0; i < placas_.size(); ++i)
        {
            if (placas_[i].getGlobalBounds().contains(punto))
            {
                seleccion_ = i;
                activar();
                return;
            }
        }
    }
}

void PantallaOpciones::actualizar(float dt)
{
    reloj_ += dt;

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        const bool elegida = (i == seleccion_);

        placas_[i].setFillColor(elegida ? tema::kBotonHover : tema::kBoton);
        placas_[i].setOutlineThickness(2.f);
        placas_[i].setOutlineColor(elegida ? kAmarillo : tema::kPanelBorde);

        etiquetas_[i].setFillColor(elegida ? tema::kTexto : tema::kTextoSuave);
        valores_[i].setFillColor(elegida ? kAmarillo : tema::kTextoSuave);
    }

    const sf::FloatRect placa = placas_[seleccion_].getGlobalBounds();
    const float empuje = std::sin(reloj_ * 5.f) * 4.f;
    senalador_.setPosition({ placa.position.x - 30.f + empuje,
                             placa.position.y + placa.size.y / 2.f - 8.f });
}

void PantallaOpciones::dibujar(sf::RenderTarget& objetivo) const
{
    if (fondo_) objetivo.draw(*fondo_);
    objetivo.draw(velo_);

    marquesina_.dibujar(objetivo);
    tema::dibujarConSombra(objetivo, titulo_, 3.f);

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        objetivo.draw(placas_[i]);
        objetivo.draw(etiquetas_[i]);

        if (static_cast<Fila>(i) == Fila::Volumen)
        {
            objetivo.draw(fondoVolumen_);
            objetivo.draw(barraVolumen_);
        }

        objetivo.draw(valores_[i]);
    }

    objetivo.draw(senalador_);

    tiraPie_.dibujar(objetivo);
    objetivo.draw(pie_);

    objetivo.draw(barrido_);
}

}
