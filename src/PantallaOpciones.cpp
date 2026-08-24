#include "PantallaOpciones.hpp"

#include "GestorGuardado.hpp"

#include <filesystem>
#include <string>

namespace vp {

namespace {

const sf::Color kAmarillo(255, 220, 80);
const sf::Color kFondo(38, 26, 54);

constexpr unsigned kTamTitulo = 44;
constexpr unsigned kTamFila   = 30;
constexpr float    kSepFila   = 26.f;

} // namespace sin nombre

PantallaOpciones::PantallaOpciones(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , titulo_(fuente, "OPCIONES", kTamTitulo)
    , pie_(fuente, "Izquierda y derecha ajustan   -   Escape vuelve al menu", 16)
{
    velo_.setSize(tamanoVentana_);
    velo_.setFillColor(kFondo);

    titulo_.setFillColor(kAmarillo);
    const sf::FloatRect lt = titulo_.getLocalBounds();
    titulo_.setOrigin({ lt.position.x + lt.size.x / 2.f, lt.position.y });
    titulo_.setPosition({ tamanoVentana_.x / 2.f, 90.f });

    pie_.setFillColor(sf::Color(255, 255, 255, 130));
    const sf::FloatRect lp = pie_.getLocalBounds();
    pie_.setOrigin({ lp.position.x + lp.size.x / 2.f, 0.f });
    pie_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y - 46.f });

    const auto total = static_cast<std::size_t>(Fila::Total);
    filas_.reserve(total);
    for (std::size_t i = 0; i < total; ++i)
        filas_.emplace_back(fuente_, "", kTamFila);

    if (musica_.load("assets/sound/MenuOpciones.ogg"))
    {
        musica_.setLoop(true);
        musica_.setVolume(static_cast<float>(volumen_));
        musica_.play();
    }

    refrescarTextos();
}

void PantallaOpciones::refrescarTextos()
{
    const std::string etiquetas[] = {
        std::string("Musica:  ") + (musicaActiva_ ? "activada" : "silenciada"),
        "Volumen:  " + std::to_string(volumen_),
        borrada_ ? "Partida borrada" : "Borrar partida guardada",
        "Volver"
    };

    const float altoTotal = filas_.size() * kTamFila + (filas_.size() - 1) * kSepFila;
    const float inicioY   = (tamanoVentana_.y - altoTotal) / 2.f;

    for (std::size_t i = 0; i < filas_.size(); ++i)
    {
        filas_[i].setString(etiquetas[i]);

        const sf::FloatRect limites = filas_[i].getLocalBounds();
        filas_[i].setOrigin({ limites.position.x + limites.size.x / 2.f, limites.position.y });
        filas_[i].setPosition({ tamanoVentana_.x / 2.f,
                                inicioY + i * (kTamFila + kSepFila) });
    }
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
        const int total = static_cast<int>(filas_.size());

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
        for (std::size_t i = 0; i < filas_.size(); ++i)
            if (filas_[i].getGlobalBounds().contains(punto))
                seleccion_ = i;
        return;
    }

    if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
    {
        if (clic->button != sf::Mouse::Button::Left) return;

        const sf::Vector2f punto(clic->position);
        for (std::size_t i = 0; i < filas_.size(); ++i)
        {
            if (filas_[i].getGlobalBounds().contains(punto))
            {
                seleccion_ = i;
                activar();
                return;
            }
        }
    }
}

void PantallaOpciones::actualizar(float)
{
    for (std::size_t i = 0; i < filas_.size(); ++i)
        filas_[i].setFillColor(i == seleccion_ ? kAmarillo : sf::Color::White);
}

void PantallaOpciones::dibujar(sf::RenderTarget& objetivo) const
{
    objetivo.draw(velo_);
    objetivo.draw(titulo_);
    for (const sf::Text& fila : filas_)
        objetivo.draw(fila);
    objetivo.draw(pie_);
}

} // namespace vp
