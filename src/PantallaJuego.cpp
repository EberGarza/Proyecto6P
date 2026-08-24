#include "PantallaJuego.hpp"

#include "Tema.hpp"
#include "Castor.hpp"
#include "Conejo.hpp"
#include "GestorGuardado.hpp"
#include "Mosaico.hpp"
#include "RenderSprite.hpp"
#include "Utilidades.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <utility>

namespace vp {
namespace {

constexpr float kAltoBotonera   = 110.f;

} // namespace sin nombre

PantallaJuego::PantallaJuego(const sf::Font& fuente,
                             std::unique_ptr<Mascota> mascota,
                             sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , mascota_(std::move(mascota))
    , inventario_(Inventario::inicial())
    , vista_(fuente)
    , hud_(fuente, tamanoVentana, Hud::kAlturaTira)
    , panelAdmin_(fuente, tamanoVentana)
    , panelComida_(fuente, tamanoVentana)
    , tituloMetrica_(fuente, "TELEMETRIA", tema::kTextoChico)
    , textoMetrica_(fuente, "", tema::kTextoChico)
    , pie_(fuente, "TECLAS 1-6  ACCIONES     ESC  SALIR", 13)
    , anuncio_(fuente, "", 64)
    , tamanoVentana_(tamanoVentana)
{
    construirEscenario();
    construirBarrido();
    construirMetrica();

    // El escenario ocupa la franja entre el marcador y la botonera.
    const float altoEscenario = tamanoVentana.y - Hud::kAlturaTira - kAltoBotonera;
    centroEscenario_ = { tamanoVentana.x * 0.62f,
                         Hud::kAlturaTira + altoEscenario * 0.46f };

    vista_.prepararMascota(*mascota_);
    vista_.establecerEscala(3.f);   // escala entera; a x4 no cabe en el escenario nuevo
    vista_.establecerPosicion(centroEscenario_);

    // --- Botonera, con su panel de fondo ---
    panelBotonera_ = tema::panelBiselado(
        { 0.f, tamanoVentana.y - kAltoBotonera },
        { tamanoVentana.x, kAltoBotonera });

    pie_.setFillColor(tema::kTextoTenue);
    pie_.setPosition({ 22.f, tamanoVentana.y - 24.f });

    anuncio_.setFillColor(tema::kAcento);

    crearBotones();
    panelAdmin_.enlazar(admin_, *mascota_, inventario_, vista_);
    estadoAnunciado_ = mascota_->tipoEstado();
}

void PantallaJuego::construirEscenario()
{
    // El mismo mosaico del menu y de las opciones, pero con la hoja de ESTA
    // mascota: el fondo cambia con la especie y el genero elegidos.
    if (!mascota_) return;

    if (!componerMosaico(lienzoEscenario_, tamanoVentana_,
                         { RenderSprite::rutaDe(mascota_->claveArte()) },
                         tema::kFondo,
                         13,      // mas tenue que en el menu: aqui hay que ver a la mascota
                         150.f))
        return;

    escenario_.emplace(lienzoEscenario_.getTexture());
}

void PantallaJuego::construirBarrido()
{
    barrido_ = tema::barridoCRT(tamanoVentana_);
}

void PantallaJuego::anunciar(const std::string& texto, sf::Color color)
{
    anuncio_.setString(texto);
    anuncio_.setFillColor(color);

    const sf::FloatRect limites = anuncio_.getLocalBounds();
    anuncio_.setOrigin({ limites.position.x + limites.size.x / 2.f,
                         limites.position.y + limites.size.y / 2.f });
    anuncio_.setPosition({ tamanoVentana_.x / 2.f, Hud::kAlturaTira + 70.f });
    anuncioRestante_ = 1.4f;
}

void PantallaJuego::crearBotones()
{
    // Botonera de siete teclas repartidas a lo ancho, como el panel de control
    // de una recreativa.
    const float margen = 18.f;
    const float hueco  = 8.f;
    const int   total  = 7;          // seis acciones comunes y la de la especie
    const float anchoBoton = (tamanoVentana_.x - margen * 2.f - hueco * (total - 1)) / total;

    const sf::Vector2f tamano(anchoBoton, 52.f);
    float x = margen;
    const float y = tamanoVentana_.y - kAltoBotonera + 16.f;

    Mascota* m = mascota_.get();
    int indiceTecla = 1;

    const auto agregar = [&](const std::string& etiqueta, Boton::Accion accion, bool conTecla)
    {
        botones_.emplace_back(fuente_, etiqueta, sf::Vector2f(x, y), tamano,
                              std::move(accion));
        if (conTecla) botones_.back().establecerTecla(std::to_string(indiceTecla++));
        x += tamano.x + hueco;
    };

    // Las acciones con objeto salen del inventario, no de un numero fijo: asi
    // cada comida alimenta lo suyo y se gasta al usarse.
    agregar("ALIMENTAR", [this] { abrirDespensa(); },            true);
    agregar("JUGAR",     [this] { usarPrimero("Juguete"); },     true);
    agregar("ASEAR",     [this] { usarPrimero("Aseo"); },        true);
    agregar("MEDICAR",   [this] { usarPrimero("Medicina"); },    true);

    agregar("DORMIR",    [m] { if (m->tipoEstado() == TipoEstado::Durmiendo) m->despertar();
                               else                                          m->dormir(); }, true);
    agregar("ACARICIAR", [m] { m->acariciar(); },     true);

    // Accion exclusiva de cada especie: se resuelve con dynamic_cast, que es
    // la forma segura de preguntar por el tipo concreto detras del puntero base.
    if (auto* conejo = dynamic_cast<Conejo*>(m))
        agregar("SALTAR", [conejo] { conejo->saltar(); }, false);
    else if (auto* castor = dynamic_cast<Castor*>(m))
        agregar("ROER", [castor] { castor->roer(); }, false);
}

// ------------------------------------------------------------- Inventario ---

void PantallaJuego::abrirDespensa()
{
    if (!mascota_->estaViva()) return;

    panelComida_.refrescar(inventario_, [this](std::size_t indice)
    {
        inventario_.usar(indice, *mascota_);
        inventario_.limpiarAgotados();
    });
    panelComida_.abrir();
}

void PantallaJuego::usarPrimero(const std::string& categoria)
{
    // Para juguete, medicina y jabon no hace falta elegir: se coge el primero
    // que quede de esa categoria. La despensa solo se abre para la comida,
    // que es donde la eleccion cambia algo.
    const auto indices = inventario_.indicesPorCategoria(categoria);
    if (indices.empty())
    {
        mascota_->registrar("No queda nada de " + categoria + ".");
        return;
    }

    inventario_.usar(indices.front(), *mascota_);
    inventario_.limpiarAgotados();
}

void PantallaJuego::construirMetrica()
{
    const sf::Vector2f tamano(228.f, 96.f);
    const sf::Vector2f pos(tamanoVentana_.x - tamano.x - 20.f, Hud::kAlturaTira + 14.f);

    panelMetrica_ = tema::panelBiselado(pos, tamano, tema::kPanelBorde);

    tituloMetrica_.setFillColor(tema::kAdmin);
    tituloMetrica_.setPosition({ pos.x + 12.f, pos.y + 8.f });

    textoMetrica_.setFillColor(tema::kTexto);
    textoMetrica_.setLineSpacing(1.3f);
    textoMetrica_.setPosition({ pos.x + 12.f, pos.y + 30.f });

    // Barrita de avance de la actividad, al pie del panel.
    fondoProgreso_ = tema::paralelogramo({ pos.x + 12.f, pos.y + tamano.y - 20.f },
                                         { tamano.x - 24.f, 10.f }, 4.f);
    fondoProgreso_.setFillColor(tema::kBarraFondo);
    origenProgreso_ = { pos.x + 12.f, pos.y + tamano.y - 20.f };
    anchoProgreso_  = tamano.x - 24.f;
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

        // Los paneles estan por encima de la botonera: si hay uno abierto, se
        // queda el clic para que no se cuele a los botones de debajo.
        if (admin_.visible())      { panelAdmin_.procesarClic(punto);  return; }
        if (panelComida_.visible()) { panelComida_.procesarClic(punto); return; }

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
        // Con la despensa abierta, los numeros eligen comida en vez de lanzar
        // las acciones de siempre.
        if (panelComida_.visible())
        {
            switch (tecla->code)
            {
                case sf::Keyboard::Key::Num1: panelComida_.elegirPorIndice(0); break;
                case sf::Keyboard::Key::Num2: panelComida_.elegirPorIndice(1); break;
                case sf::Keyboard::Key::Num3: panelComida_.elegirPorIndice(2); break;
                case sf::Keyboard::Key::Num4: panelComida_.elegirPorIndice(3); break;
                case sf::Keyboard::Key::Escape: panelComida_.cerrar();         break;
                default: break;
            }
            return;
        }

        switch (tecla->code)
        {
            case sf::Keyboard::Key::F1:   admin_.alternarVisible();  break;

            case sf::Keyboard::Key::Escape:
                if (admin_.visible()) admin_.cerrar();
                else                  solicitar(Transicion::Salir);
                break;

            case sf::Keyboard::Key::Num1: abrirDespensa();           break;
            case sf::Keyboard::Key::Num2: usarPrimero("Juguete");    break;
            case sf::Keyboard::Key::Num3: usarPrimero("Aseo");       break;
            case sf::Keyboard::Key::Num4: usarPrimero("Medicina");   break;

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
    hud_.actualizar(*mascota_, dt);

    // Cuando la mascota cambia de estado se lanza un cartel en mitad del
    // escenario, como los avisos de asalto de una recreativa. K.O. al morir.
    if (mascota_->tipoEstado() != estadoAnunciado_)
    {
        estadoAnunciado_ = mascota_->tipoEstado();
        if (estadoAnunciado_ == TipoEstado::Muerta)
            anunciar("K.O.", tema::kMal);
        else
            anunciar(mascota_->estado().nombre(), tema::kAcento);
    }

    if (anuncioRestante_ > 0.f)
    {
        anuncioRestante_ -= dt;

        // Entra de golpe y se desvanece al final.
        const float t = anuncioRestante_ / 1.4f;
        const auto alfa = static_cast<std::uint8_t>(255.f * std::min(1.f, t * 3.f));
        sf::Color color = anuncio_.getFillColor();
        color.a = alfa;
        anuncio_.setFillColor(color);

        const float escala = 1.f + (1.f - std::min(1.f, (1.4f - anuncioRestante_) * 6.f)) * 0.4f;
        anuncio_.setScale({ escala, escala });
    }

    panelComida_.actualizar(raton_);

    if (admin_.visible())
        panelAdmin_.actualizar(raton_, admin_, *mascota_, vista_, fps_);

    if (admin_.metricaVisible())
        actualizarMetrica();
}

void PantallaJuego::actualizarMetrica()
{
    const Actividad actividad = mascota_->actividad();
    const float     avance    = mascota_->progresoActividad();

    textoMetrica_.setString(
        "Actividad: " + nombreActividad(actividad) + "\n" +
        "Estado:    " + mascota_->estado().nombre() + "\n" +
        "Avance:    " + util::aTexto(avance * 100.f) + "%");

    // La barrita solo tiene sentido en las actividades con cuenta atras:
    // durmiendo no hay un final calculable, asi que se deja vacia.
    barraProgreso_ = tema::paralelogramo(origenProgreso_,
                                         { anchoProgreso_ * avance, 10.f }, 4.f);
    barraProgreso_.setFillColor(mascota_->ocupada() ? tema::kAcento : tema::kBarraFondo);
}

// --------------------------------------------------------------- Dibujado ---

void PantallaJuego::dibujar(sf::RenderTarget& objetivo) const
{
    if (escenario_) objetivo.draw(*escenario_);
    objetivo.draw(vista_);

    if (anuncioRestante_ > 0.f)
        objetivo.draw(anuncio_);

    objetivo.draw(hud_);

    // Telemetria: la enciende el Admin_Menu y se queda aunque el menu se cierre.
    if (admin_.metricaVisible())
    {
        panelMetrica_.dibujar(objetivo);
        tema::dibujarConSombra(objetivo, tituloMetrica_);
        objetivo.draw(textoMetrica_);
        objetivo.draw(fondoProgreso_);
        objetivo.draw(barraProgreso_);
    }

    panelBotonera_.dibujar(objetivo);
    for (const Boton& boton : botones_)
        objetivo.draw(boton);

    objetivo.draw(pie_);

    // El barrido va encima de todo el juego, pero por debajo de los paneles:
    // ahi estorbaria para leer los numeros.
    objetivo.draw(barrido_);

    objetivo.draw(panelComida_);

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
