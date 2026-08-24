#include "PantallaJuego.hpp"

#include "Tema.hpp"
#include "Castor.hpp"
#include "Conejo.hpp"
#include "GestorGuardado.hpp"
#include "HojaSprites.hpp"
#include "RenderSprite.hpp"

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
    , pie_(fuente, "TECLAS 1-6  ACCIONES     ESC  SALIR", 13)
    , anuncio_(fuente, "", 64)
    , tamanoVentana_(tamanoVentana)
{
    construirEscenario();
    construirBarrido();

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
    const auto ancho = static_cast<unsigned>(tamanoVentana_.x);
    const auto alto  = static_cast<unsigned>(tamanoVentana_.y);
    if (!lienzoEscenario_.resize({ ancho, alto })) return;

    lienzoEscenario_.clear(tema::kFondo);

    // Un patron repetido con la propia mascota, como el del menu de inicio.
    // Al usar su hoja, el fondo cambia con la especie y el genero elegidos.
    HojaSprites hoja;
    if (mascota_ && hoja.cargar(RenderSprite::rutaDe(mascota_->claveArte())))
    {
        if (const Animacion* idle = hoja.animacion(TipoEstado::Normal))
        {
            sf::Sprite sello(hoja.textura());
            idle->aplicarCuadroActual(sello);
            sello.setColor(sf::Color(255, 255, 255, 13));
            sello.setScale({ 2.f, 2.f });

            // El paso va con el tamano del sello (64 px por 2), con aire de
            // sobra: si se acercan mas, el patron deja de leerse como fondo.
            const float paso = 150.f;
            int fila = 0;
            for (float y = 20.f; y < tamanoVentana_.y + paso; y += paso, ++fila)
            {
                const float desfase = (fila % 2 == 0) ? 0.f : paso / 2.f;
                for (float x = 20.f + desfase; x < tamanoVentana_.x + paso; x += paso)
                {
                    sello.setRotation(sf::degrees(((fila + static_cast<int>(x)) % 5) * 7.f - 14.f));
                    sello.setPosition({ x, y });
                    lienzoEscenario_.draw(sello);
                }
            }
        }
    }

    // Antes habia aqui una linea de suelo dibujada a mano. Ya no hace falta:
    // los sprites nuevos traen su propia sombra, y la linea quedaba a otra
    // altura, como si la mascota flotara por encima del suelo.

    lienzoEscenario_.display();
    escenario_.emplace(lienzoEscenario_.getTexture());
}

void PantallaJuego::construirBarrido()
{
    // Lineas horizontales oscuras cada dos pixeles. Es el truco mas barato que
    // hay para que una pantalla parezca un monitor de tubo.
    barrido_.setPrimitiveType(sf::PrimitiveType::Lines);
    barrido_.clear();

    for (float y = 0.f; y < tamanoVentana_.y; y += 3.f)
    {
        const sf::Color tinta(0, 0, 0, 46);
        barrido_.append(sf::Vertex{ { 0.f, y }, tinta });
        barrido_.append(sf::Vertex{ { tamanoVentana_.x, y }, tinta });
    }
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

    VistaMascota* vista = &vista_;

    agregar("ALIMENTAR", [m] { m->alimentar(25.f); }, true);
    agregar("JUGAR",     [m] { m->jugar(20.f); },     true);

    // Asear es la unica accion con animacion propia en la hoja: al pulsarla la
    // mascota se bana un momento y despues vuelve a su estado. Si la hoja no
    // trae esa animacion, reproducirAccion no hace nada y la accion funciona
    // igual, solo que sin dibujo especial.
    agregar("ASEAR",     [m, vista] { if (m->asear(40.f)) vista->reproducirAccion("Aseo"); },
            true);

    agregar("MEDICAR",   [m] { m->medicar(35.f); },   true);
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
            case sf::Keyboard::Key::Num3:
                if (mascota_->asear(40.f)) vista_.reproducirAccion("Aseo");
                break;
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

    if (admin_.visible())
        panelAdmin_.actualizar(raton_, admin_, *mascota_, vista_, fps_);
}

// --------------------------------------------------------------- Dibujado ---

void PantallaJuego::dibujar(sf::RenderTarget& objetivo) const
{
    if (escenario_) objetivo.draw(*escenario_);
    objetivo.draw(vista_);

    if (anuncioRestante_ > 0.f)
        objetivo.draw(anuncio_);

    objetivo.draw(hud_);

    panelBotonera_.dibujar(objetivo);
    for (const Boton& boton : botones_)
        objetivo.draw(boton);

    objetivo.draw(pie_);

    // El barrido va encima de todo el juego, pero por debajo del panel de
    // desarrollo: ahi estorbaria para leer los numeros.
    objetivo.draw(barrido_);

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
