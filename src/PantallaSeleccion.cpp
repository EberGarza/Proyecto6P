#include "PantallaSeleccion.hpp"

#include "Tema.hpp"
#include "GestorGuardado.hpp"

#include <cstddef>
#include <utility>

namespace vp {

PantallaSeleccion::PantallaSeleccion(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , vista_(fuente)
    , titulo_        (fuente, "VirtualPet", 34)
    , subtitulo_     (fuente, "Elige a tu companera y ponle nombre", tema::kTextoNormal)
    , etiquetaNombre_(fuente, "NOMBRE", tema::kTextoChico)
    , textoNombre_   (fuente, "", tema::kTextoTitulo)
    , descripcion_   (fuente, "", tema::kTextoChico)
    , ayuda_(fuente,
             "Escribe para cambiar el nombre   |   Enter: comenzar   |   ESC: salir",
             tema::kTextoChico)
{
    hayPartida_ = GestorGuardado::existePartida(GestorGuardado::rutaPorDefecto());
    nombre_     = "Fantasmin";

    construirInterfaz();
    elegirEspecie(TipoMascota::Gastly);
}

void PantallaSeleccion::centrar(sf::Text& texto, float y) const
{
    // En SFML 3 getLocalBounds() devuelve position y size en vez de
    // left/top/width/height.
    const sf::FloatRect limites = texto.getLocalBounds();
    texto.setOrigin({ limites.position.x + limites.size.x * 0.5f, limites.position.y });
    texto.setPosition({ tamanoVentana_.x * 0.5f, y });
}

void PantallaSeleccion::construirInterfaz()
{
    titulo_.setFillColor(tema::kTexto);
    subtitulo_.setFillColor(tema::kTextoSuave);
    etiquetaNombre_.setFillColor(tema::kTextoSuave);
    textoNombre_.setFillColor(tema::kTexto);
    descripcion_.setFillColor(tema::kTextoSuave);
    ayuda_.setFillColor(tema::kTextoTenue);

    centrar(titulo_,    36.f);
    centrar(subtitulo_, 82.f);

    // Recuadro donde se ve la mascota elegida.
    const sf::Vector2f tamanoPanel(320.f, 240.f);
    panelPrevio_.setSize(tamanoPanel);
    panelPrevio_.setPosition({ (tamanoVentana_.x - tamanoPanel.x) * 0.5f, 190.f });
    panelPrevio_.setFillColor(tema::kPanel);
    panelPrevio_.setOutlineThickness(1.f);
    panelPrevio_.setOutlineColor(tema::kPanelBorde);

    vista_.establecerEscala(2.f);   // entero, igual que en la partida
    vista_.establecerPosicion({ tamanoVentana_.x * 0.5f, 190.f + tamanoPanel.y * 0.55f });

    // Campo de texto del nombre.
    const sf::Vector2f tamanoCampo(320.f, 46.f);
    campoNombre_.setSize(tamanoCampo);
    campoNombre_.setPosition({ (tamanoVentana_.x - tamanoCampo.x) * 0.5f, 496.f });
    campoNombre_.setFillColor(tema::kFondo);
    campoNombre_.setOutlineThickness(1.f);
    campoNombre_.setOutlineColor(tema::kAcento);

    etiquetaNombre_.setPosition({ campoNombre_.getPosition().x, 476.f });

    // --- Botones de especie ---
    const auto tipos = FabricaMascotas::tiposDisponibles();
    const sf::Vector2f tamanoBoton(140.f, 44.f);
    const float cuantos = static_cast<float>(tipos.size());
    const float ancho   = cuantos * tamanoBoton.x + (cuantos - 1.f) * 14.f;
    float x = (tamanoVentana_.x - ancho) * 0.5f;

    for (TipoMascota tipo : tipos)
    {
        botones_.emplace_back(fuente_, FabricaMascotas::nombreTipo(tipo),
                              sf::Vector2f(x, 120.f), tamanoBoton,
                              [this, tipo] { elegirEspecie(tipo); });
        x += tamanoBoton.x + 14.f;
    }

    // --- Comenzar y continuar ---
    const sf::Vector2f tamanoAccion(200.f, 48.f);
    botones_.emplace_back(fuente_, "Comenzar",
                          sf::Vector2f((tamanoVentana_.x - tamanoAccion.x) * 0.5f, 562.f),
                          tamanoAccion, [this] { confirmar(); });
    botones_.back().establecerColores(tema::kAcento, tema::kBotonHover);

    if (hayPartida_)
    {
        botones_.emplace_back(fuente_, "Continuar partida",
                              sf::Vector2f(tamanoVentana_.x - 240.f, 36.f),
                              sf::Vector2f(200.f, 40.f),
                              [this] { continuarPartida(); });
        botones_.back().establecerTamanoTexto(tema::kTextoChico);
    }

    centrar(ayuda_, tamanoVentana_.y - 34.f);
}

// --------------------------------------------------------------- Acciones ---

void PantallaSeleccion::elegirEspecie(TipoMascota tipo)
{
    mascota_ = FabricaMascotas::crear(tipo, nombre_.empty() ? "Sin nombre" : nombre_);
    if (!mascota_) return;

    vista_.prepararEspecie(mascota_->especie());
    descripcion_.setString(mascota_->descripcion());

    // La descripcion se parte en dos lineas si no cabe en el ancho del panel.
    if (descripcion_.getLocalBounds().size.x > tamanoVentana_.x - 200.f)
    {
        std::string texto = mascota_->descripcion();
        const std::size_t mitad = texto.find(' ', texto.size() / 2);
        if (mitad != std::string::npos) texto[mitad] = '\n';
        descripcion_.setString(texto);
    }
    centrar(descripcion_, 440.f);

    // Se resalta el boton de la especie elegida.
    const auto tipos = FabricaMascotas::tiposDisponibles();
    for (std::size_t i = 0; i < tipos.size() && i < botones_.size(); ++i)
        botones_[i].establecerActivo(tipos[i] == tipo);
}

void PantallaSeleccion::confirmar()
{
    if (!mascota_) return;

    mascota_->establecerNombre(nombre_.empty() ? "Sin nombre" : nombre_);
    solicitar(Transicion::Juego);
}

void PantallaSeleccion::continuarPartida()
{
    auto cargada = GestorGuardado::cargar(GestorGuardado::rutaPorDefecto());
    if (!cargada) return;

    mascota_ = std::move(cargada);
    solicitar(Transicion::Juego);
}

void PantallaSeleccion::escribir(char32_t caracter)
{
    if (caracter == 8)                       // retroceso
    {
        if (!nombre_.empty()) nombre_.pop_back();
    }
    else if (caracter == 13 || caracter == 10)   // Enter
    {
        confirmar();
        return;
    }
    else if (caracter >= 32 && caracter < 127 && nombre_.size() < kMaxNombre)
    {
        nombre_ += static_cast<char>(caracter);
    }

    if (mascota_) mascota_->establecerNombre(nombre_.empty() ? "Sin nombre" : nombre_);
}

std::unique_ptr<Mascota> PantallaSeleccion::tomarMascota()
{
    return std::move(mascota_);
}

// ---------------------------------------------------------------- Eventos ---

void PantallaSeleccion::manejarEvento(const sf::Event& evento)
{
    if (evento.is<sf::Event::Closed>())
    {
        solicitar(Transicion::Salir);
        return;
    }

    if (const auto* escrito = evento.getIf<sf::Event::TextEntered>())
    {
        if (escrito->unicode < 128) escribir(escrito->unicode);
        return;
    }

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {
        if (tecla->code == sf::Keyboard::Key::Escape) solicitar(Transicion::Salir);
        return;
    }

    if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
    {
        if (clic->button != sf::Mouse::Button::Left) return;

        const sf::Vector2f punto(clic->position);
        for (Boton& boton : botones_)
            if (boton.procesarClic(punto)) break;
        return;
    }

    if (const auto* movimiento = evento.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2f punto(movimiento->position);
        for (Boton& boton : botones_)
            boton.actualizar(punto);
    }
}

// ----------------------------------------------------------- Actualizacion --

void PantallaSeleccion::actualizar(float dt)
{
    relojCursor_ += dt;
    if (relojCursor_ > 1.f) relojCursor_ = 0.f;

    // Cursor parpadeante al final del nombre.
    const bool cursorVisible = relojCursor_ < 0.5f;
    textoNombre_.setString(nombre_ + (cursorVisible ? "|" : " "));

    const sf::FloatRect limites = textoNombre_.getLocalBounds();
    textoNombre_.setOrigin({ limites.position.x + limites.size.x * 0.5f, limites.position.y });
    textoNombre_.setPosition({ campoNombre_.getPosition().x + campoNombre_.getSize().x * 0.5f,
                               campoNombre_.getPosition().y + 10.f });

    if (mascota_) vista_.actualizar(*mascota_, dt);
}

// --------------------------------------------------------------- Dibujado ---

void PantallaSeleccion::dibujar(sf::RenderTarget& objetivo) const
{
    objetivo.draw(titulo_);
    objetivo.draw(subtitulo_);

    for (const Boton& boton : botones_)
        objetivo.draw(boton);

    objetivo.draw(panelPrevio_);
    objetivo.draw(vista_);
    objetivo.draw(descripcion_);

    objetivo.draw(etiquetaNombre_);
    objetivo.draw(campoNombre_);
    objetivo.draw(textoNombre_);
    objetivo.draw(ayuda_);
}

} // namespace vp
