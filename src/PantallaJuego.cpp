#include "PantallaJuego.hpp"

#include "Tema.hpp"
#include "Dragon.hpp"
#include "Gastly.hpp"
#include "Gato.hpp"
#include "GestorGuardado.hpp"

#include <utility>

namespace vp {
namespace {

constexpr float kAnchoHud       = 320.f;
constexpr float kAltoBotonera   = 110.f;

} // namespace sin nombre

PantallaJuego::PantallaJuego(const sf::Font& fuente,
                             std::unique_ptr<Mascota> mascota,
                             sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , mascota_(std::move(mascota))
    , inventario_(Inventario::inicial())
    , vista_(fuente)
    , hud_(fuente,
           { tema::kMargen, tema::kMargen },
           { kAnchoHud, tamanoVentana.y - kAltoBotonera - tema::kMargen * 2.f })
    , panelAdmin_(fuente, tamanoVentana)
    , pie_(fuente, "Teclas 1-6: acciones   |   ESC: salir", tema::kTextoChico)
    , tamanoVentana_(tamanoVentana)
{
    const float xEscenario = tema::kMargen * 2.f + kAnchoHud;
    const float anchoEscenario = tamanoVentana.x - xEscenario - tema::kMargen;
    const float altoEscenario  = tamanoVentana.y - kAltoBotonera - tema::kMargen * 2.f;

    escenario_.setPosition({ xEscenario, tema::kMargen });
    escenario_.setSize({ anchoEscenario, altoEscenario });
    escenario_.setFillColor(tema::kPanel);
    escenario_.setOutlineThickness(1.f);
    escenario_.setOutlineColor(tema::kPanelBorde);

    vista_.prepararEspecie(mascota_->especie());
    // Factor entero: ampliar pixel art por un numero fraccionario hace que unos
    // pixeles ocupen 3 y otros 4, y eso produce hormigueo al animarse.
    vista_.establecerEscala(4.f);
    vista_.establecerPosicion({ xEscenario + anchoEscenario * 0.5f,
                                tema::kMargen + altoEscenario * 0.55f });

    pie_.setFillColor(tema::kTextoTenue);
    pie_.setPosition({ tema::kMargen, tamanoVentana.y - 26.f });

    crearBotones();
    panelAdmin_.enlazar(admin_, *mascota_, inventario_, vista_);
}

void PantallaJuego::crearBotones()
{
    // Con la accion exclusiva de especie llegan a ser siete botones, asi que
    // el ancho esta ajustado para que la fila entre completa en la ventana.
    const sf::Vector2f tamano(120.f, 42.f);
    float x = tema::kMargen;
    const float y = tamanoVentana_.y - kAltoBotonera + 10.f;

    Mascota* m = mascota_.get();

    const auto agregar = [&](const std::string& etiqueta, Boton::Accion accion)
    {
        botones_.emplace_back(fuente_, etiqueta, sf::Vector2f(x, y), tamano,
                              std::move(accion));
        x += tamano.x + 10.f;
    };

    agregar("Alimentar", [m] { m->alimentar(25.f); });
    agregar("Jugar",     [m] { m->jugar(20.f); });
    agregar("Asear",     [m] { m->asear(40.f); });
    agregar("Medicar",   [m] { m->medicar(35.f); });
    agregar("Dormir",    [m] { if (m->tipoEstado() == TipoEstado::Durmiendo) m->despertar();
                               else                                          m->dormir(); });
    agregar("Acariciar", [m] { m->acariciar(); });

    // Accion exclusiva de cada especie: se resuelve con dynamic_cast, que es
    // la forma segura de preguntar por el tipo concreto detras del puntero base.
    if (auto* gastly = dynamic_cast<Gastly*>(m))
        agregar("Asustar", [gastly] { gastly->asustar(); });
    else if (auto* dragon = dynamic_cast<Dragon*>(m))
        agregar("Escupir fuego", [dragon] { dragon->escupirFuego(); });
    else if (auto* gato = dynamic_cast<Gato*>(m))
        agregar("Acicalarse", [gato] { gato->acicalarse(); });
}

// ---------------------------------------------------------------- Eventos ---

// En SFML 3 sf::Event ya no es una union con un campo .type: es un tipo suma.
// Se pregunta por cada variante con is<T>() o se pide con getIf<T>(), que
// devuelve un puntero al dato solo si el evento es de ese tipo. Es mas verboso
// pero impide leer los campos del evento equivocado.
void PantallaJuego::manejarEvento(const sf::Event& evento)
{
    if (evento.is<sf::Event::Closed>())
    {
        solicitar(Transicion::Salir);
        return;
    }

    if (const auto* movimiento = evento.getIf<sf::Event::MouseMoved>())
    {
        raton_ = sf::Vector2f(movimiento->position);
        return;
    }

    if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
    {
        if (clic->button != sf::Mouse::Button::Left) return;

        const sf::Vector2f punto(clic->position);

        // El Admin_Menu esta por encima: si esta abierto, se queda el clic.
        if (admin_.visible())
        {
            panelAdmin_.procesarClic(punto);
            return;
        }

        for (Boton& boton : botones_)
            if (boton.procesarClic(punto)) break;
        return;
    }

    if (const auto* escrito = evento.getIf<sf::Event::TextEntered>())
    {
        // Cada letra alimenta la secuencia secreta del Admin_Menu.
        if (escrito->unicode < 128)
            admin_.registrarTecla(static_cast<char>(escrito->unicode));
        return;
    }

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {
        switch (tecla->code)
        {
            case sf::Keyboard::Key::F1:   admin_.alternarVisible();  break;

            case sf::Keyboard::Key::Escape:
                if (admin_.visible()) admin_.cerrar();
                else                  solicitar(Transicion::Salir);
                break;

            case sf::Keyboard::Key::Num1: mascota_->alimentar(25.f); break;
            case sf::Keyboard::Key::Num2: mascota_->jugar(20.f);     break;
            case sf::Keyboard::Key::Num3: mascota_->asear(40.f);     break;
            case sf::Keyboard::Key::Num4: mascota_->medicar(35.f);   break;

            case sf::Keyboard::Key::Num5:
                if (mascota_->tipoEstado() == TipoEstado::Durmiendo) mascota_->despertar();
                else                                                 mascota_->dormir();
                break;

            case sf::Keyboard::Key::Num6: mascota_->acariciar();     break;

            default: break;
        }
    }
}

// ----------------------------------------------------------- Actualizacion --

void PantallaJuego::actualizar(float dt)
{
    fps_ = (dt > 0.f) ? 1.f / dt : 0.f;

    // El Admin_Menu puede acelerar el reloj para ver las transiciones al vuelo.
    const float dtJuego = dt * admin_.escalaTiempo();

    mascota_->actualizar(dtJuego);
    admin_.aplicarPorTick(*mascota_, dtJuego);

    for (Boton& boton : botones_)
    {
        // Los botones siguen activos mientras la mascota viva: es la propia
        // Mascota la que rechaza la accion si el estado no la permite y lo
        // explica en la bitacora.
        boton.establecerHabilitado(mascota_->estaViva());
        boton.actualizar(raton_);
    }

    vista_.actualizar(*mascota_, dt);   // la animacion va en tiempo real
    hud_.actualizar(*mascota_);

    if (admin_.visible())
        panelAdmin_.actualizar(raton_, admin_, *mascota_, vista_, fps_);
}

// --------------------------------------------------------------- Dibujado ---

void PantallaJuego::dibujar(sf::RenderTarget& objetivo) const
{
    objetivo.draw(escenario_);
    objetivo.draw(vista_);
    objetivo.draw(hud_);

    for (const Boton& boton : botones_)
        objetivo.draw(boton);

    objetivo.draw(pie_);

    if (admin_.visible())
        objetivo.draw(panelAdmin_);
}

// --------------------------------------------------------------- Guardado ---

bool PantallaJuego::guardar() const
{
    if (!mascota_) return false;
    return GestorGuardado::guardar(*mascota_, GestorGuardado::rutaPorDefecto());
}

} // namespace vp
