#pragma once

#include "GestorRecursos.hpp"
#include "Pantalla.hpp"

#include <SFML/Graphics.hpp>

#include <memory>

namespace vp {

class Juego
{
public:
    Juego();
    ~Juego();

    Juego(const Juego&)            = delete;
    Juego& operator=(const Juego&) = delete;

    int ejecutar();

private:
    bool inicializar();

    bool cargarFuente();

    void cambiarPantalla(Pantalla::Transicion destino);
    void procesarEventos();
    void actualizar(float dt);
    void actualizarFundido(float dt);
    void dibujar();
    void alCerrar();

    void alternarPantallaCompleta();
    void aplicarVista();

    static constexpr unsigned kAnchoVentana    = 1024;
    static constexpr unsigned kAltoVentana     = 640;
    static constexpr float    kDtMaximo        = 0.1f;
    static constexpr float    kDuracionFundido = 0.28f;

    sf::RenderWindow            ventana_;
    GestorRecursos<sf::Font>    fuentes_;
    std::unique_ptr<Pantalla>   pantalla_;
    bool                        listo_ = false;

    sf::RectangleShape          fundido_;
    bool                        fundiendo_    = false;
    bool                        saliendo_     = false;
    float                       fundidoAlfa_  = 0.f;
    Pantalla::Transicion        transicionPendiente_ = Pantalla::Transicion::Ninguna;

    sf::View                    vistaLogica_;
    bool                        pantallaCompleta_ = false;
};

}
