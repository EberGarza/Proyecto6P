#include "Juego.hpp"

#include "PantallaJuego.hpp"
#include "PantallaMenu.hpp"
#include "PantallaOpciones.hpp"
#include "PantallaSeleccion.hpp"
#include "Tema.hpp"

#include <array>
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
                fuentes_.obtener("principal"), tamano);
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

    while (const std::optional<sf::Event> evento = ventana_.pollEvent())
    {
        if (pantalla_)                          pantalla_->manejarEvento(*evento);
        else if (evento->is<sf::Event::Closed>()) ventana_.close();
    }
}

void Juego::actualizar(float dt)
{
    if (!pantalla_) return;

    pantalla_->actualizar(dt);

    const Pantalla::Transicion pedida = pantalla_->transicion();
    if (pedida != Pantalla::Transicion::Ninguna)
    {
        pantalla_->consumirTransicion();
        cambiarPantalla(pedida);
    }
}

void Juego::dibujar()
{
    if (!ventana_.isOpen()) return;

    ventana_.clear(tema::kFondo);
    if (pantalla_) pantalla_->dibujar(ventana_);
    ventana_.display();
}

void Juego::alCerrar()
{

    if (auto* enJuego = dynamic_cast<PantallaJuego*>(pantalla_.get()))
        enJuego->guardar();
}

}
