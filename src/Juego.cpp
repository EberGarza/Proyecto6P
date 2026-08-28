#include "Juego.hpp"

#include "PantallaJuego.hpp"
#include "PantallaMenu.hpp"
#include "PantallaOpciones.hpp"
#include "PantallaSeleccion.hpp"
#include "Tema.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <utility>

namespace vp {

Juego::Juego()  = default;
Juego::~Juego() = default;

bool Juego::cargarFuente()
{

    static const std::array<const char*, 7> rutas {
        "assets/fonts/VCR_OSD_MONO_1.001.ttf",
        "assets/fonts/04B_30__.TTF",
        "assets/fonts/fuente.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };

    for (const char* ruta : rutas)
        if (fuentes_.cargar("principal", ruta))
            return true;

    std::cerr << "No se encontro ninguna fuente. Coloca un .ttf en assets/fonts/fuente.ttf\n";
    return false;
}

bool Juego::inicializar()
{

    ventana_.create(sf::VideoMode({ kAnchoVentana, kAltoVentana }), "VirtualPet");

    ventana_.setVerticalSyncEnabled(true);
    ventana_.setKeyRepeatEnabled(true);
    aplicarVista();

    fundido_.setSize({ static_cast<float>(kAnchoVentana), static_cast<float>(kAltoVentana) });
    fundido_.setFillColor(sf::Color(0, 0, 0, 0));

    if (!cargarFuente()) return false;

    cambiarPantalla(Pantalla::Transicion::Menu);

    listo_ = pantalla_ != nullptr;
    return listo_;
}

void Juego::cambiarPantalla(Pantalla::Transicion destino)
{
    const sf::Vector2f tamano(static_cast<float>(kAnchoVentana),
                              static_cast<float>(kAltoVentana));

    switch (destino)
    {
        case Pantalla::Transicion::Menu:
            pantalla_ = std::make_unique<PantallaMenu>(
                fuentes_.obtener("principal"), tamano);
            break;

        case Pantalla::Transicion::Opciones:
            pantalla_ = std::make_unique<PantallaOpciones>(
                fuentes_.obtener("principal"), tamano, pantallaCompleta_);
            break;

        case Pantalla::Transicion::Seleccion:
            pantalla_ = std::make_unique<PantallaSeleccion>(
                fuentes_.obtener("principal"), tamano);
            break;

        case Pantalla::Transicion::Juego:
        {

            auto* seleccion = dynamic_cast<PantallaSeleccion*>(pantalla_.get());
            if (!seleccion) break;

            auto mascota = seleccion->tomarMascota();
            if (!mascota) break;

            pantalla_ = std::make_unique<PantallaJuego>(
                fuentes_.obtener("principal"), std::move(mascota), tamano);
            break;
        }

        case Pantalla::Transicion::Salir:
            alCerrar();
            ventana_.close();
            break;

        case Pantalla::Transicion::Ninguna:
        case Pantalla::Transicion::AlternarPantallaCompleta:
            break;
    }
}

int Juego::ejecutar()
{
    if (!inicializar())
    {
        std::cerr << "No se pudo inicializar el juego.\n";
        return 1;
    }

    sf::Clock reloj;

    while (ventana_.isOpen())
    {
        float dt = reloj.restart().asSeconds();
        if (dt > kDtMaximo) dt = kDtMaximo;

        procesarEventos();
        if (!ventana_.isOpen()) break;

        actualizar(dt);
        dibujar();
    }

    return 0;
}

void Juego::procesarEventos()
{

    while (const std::optional<sf::Event> eventoOriginal = ventana_.pollEvent())
    {
        sf::Event evento = *eventoOriginal;

        if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>();
            tecla && tecla->code == sf::Keyboard::Key::F11)
        {
            alternarPantallaCompleta();
            continue;
        }

        if (evento.is<sf::Event::Resized>()) aplicarVista();

        if (const auto* movimiento = evento.getIf<sf::Event::MouseMoved>())
        {
            sf::Event::MouseMoved copia = *movimiento;
            copia.position = sf::Vector2i(ventana_.mapPixelToCoords(movimiento->position));
            evento = sf::Event(copia);
        }
        else if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
        {
            sf::Event::MouseButtonPressed copia = *clic;
            copia.position = sf::Vector2i(ventana_.mapPixelToCoords(clic->position));
            evento = sf::Event(copia);
        }

        if (pantalla_)                         pantalla_->manejarEvento(evento);
        else if (evento.is<sf::Event::Closed>()) ventana_.close();
    }
}

void Juego::actualizar(float dt)
{
    if (fundiendo_)
    {
        actualizarFundido(dt);
        return;
    }

    if (!pantalla_) return;

    pantalla_->actualizar(dt);

    const Pantalla::Transicion pedida = pantalla_->transicion();
    if (pedida == Pantalla::Transicion::Ninguna) return;

    pantalla_->consumirTransicion();

    if (pedida == Pantalla::Transicion::AlternarPantallaCompleta)
    {
        alternarPantallaCompleta();
        return;
    }

    if (pedida == Pantalla::Transicion::Salir)
    {
        cambiarPantalla(pedida);
        return;
    }

    transicionPendiente_ = pedida;
    fundiendo_ = true;
    saliendo_  = true;
}

void Juego::alternarPantallaCompleta()
{
    pantallaCompleta_ = !pantallaCompleta_;

    if (pantallaCompleta_)
        ventana_.create(sf::VideoMode::getDesktopMode(), "VirtualPet",
                        sf::Style::None, sf::State::Fullscreen);
    else
        ventana_.create(sf::VideoMode({ kAnchoVentana, kAltoVentana }), "VirtualPet");

    ventana_.setVerticalSyncEnabled(true);
    ventana_.setKeyRepeatEnabled(true);
    aplicarVista();
}

void Juego::aplicarVista()
{
    const sf::Vector2u tamanoReal = ventana_.getSize();
    const float anchoLogico = static_cast<float>(kAnchoVentana);
    const float altoLogico  = static_cast<float>(kAltoVentana);

    const float aspectoLogico = anchoLogico / altoLogico;
    const float aspectoReal   = static_cast<float>(tamanoReal.x) / static_cast<float>(tamanoReal.y);

    float anchoViewport = 1.f;
    float altoViewport  = 1.f;

    if (aspectoReal > aspectoLogico)
        anchoViewport = aspectoLogico / aspectoReal;
    else
        altoViewport = aspectoReal / aspectoLogico;

    vistaLogica_.setSize({ anchoLogico, altoLogico });
    vistaLogica_.setCenter({ anchoLogico / 2.f, altoLogico / 2.f });
    vistaLogica_.setViewport({ { (1.f - anchoViewport) / 2.f, (1.f - altoViewport) / 2.f },
                               { anchoViewport, altoViewport } });

    ventana_.setView(vistaLogica_);
}

void Juego::actualizarFundido(float dt)
{
    const float velocidad = 1.f / kDuracionFundido;

    if (saliendo_)
    {
        fundidoAlfa_ += dt * velocidad;
        if (fundidoAlfa_ >= 1.f)
        {
            fundidoAlfa_ = 1.f;
            cambiarPantalla(transicionPendiente_);
            transicionPendiente_ = Pantalla::Transicion::Ninguna;
            saliendo_ = false;
        }
    }
    else
    {
        fundidoAlfa_ -= dt * velocidad;
        if (fundidoAlfa_ <= 0.f)
        {
            fundidoAlfa_ = 0.f;
            fundiendo_ = false;
        }
    }
}

void Juego::dibujar()
{
    if (!ventana_.isOpen()) return;

    ventana_.clear(tema::kFondo);
    if (pantalla_) pantalla_->dibujar(ventana_);

    if (fundiendo_ || fundidoAlfa_ > 0.f)
    {
        fundido_.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(255.f * fundidoAlfa_)));
        ventana_.draw(fundido_);
    }

    ventana_.display();
}

void Juego::alCerrar()
{

    if (auto* enJuego = dynamic_cast<PantallaJuego*>(pantalla_.get()))
        enJuego->guardar();
}

}
