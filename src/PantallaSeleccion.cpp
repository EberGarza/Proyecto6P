#include "PantallaSeleccion.hpp"

#include "Tema.hpp"
#include "GestorGuardado.hpp"

#include <cstddef>
#include <utility>

namespace vp {
namespace {

/// Nombre de relleno de la vista previa, mientras el jugador no escriba uno.
/// No se guarda en la partida: solo sirve para que la descripcion se lea bien.
const char* kNombrePrevio = "Tu mascota";

constexpr float kAnchoBoton = 140.f;
constexpr float kAltoBoton  = 44.f;
constexpr float kHueco      = 10.f;
constexpr float kYBotones   = 122.f;

} // namespace sin nombre

PantallaSeleccion::PantallaSeleccion(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , vista_(fuente)
    , titulo_          (fuente, "VirtualPet", 34)
    , subtitulo_       (fuente, "Elige tu mascota y ponle nombre", tema::kTextoNormal)
    , etiquetaEspecie_ (fuente, "ESPECIE", tema::kTextoChico)
    , etiquetaGenero_  (fuente, "GENERO",  tema::kTextoChico)
    , etiquetaNombre_  (fuente, "NOMBRE",  tema::kTextoChico)
    , textoNombre_     (fuente, "", tema::kTextoTitulo)
    , descripcion_     (fuente, "", tema::kTextoChico)
    , ayuda_(fuente,
             "Flechas: especie y genero   |   Escribe el nombre   |   "
             "Enter: comenzar   |   ESC: volver",
             tema::kTextoChico)
{
    // No basta con que el archivo exista: una partida de una especie que ya no
    // esta en el juego no se puede cargar, y el boton se quedaria sin hacer
    // nada al pulsarlo. Se intenta cargar aqui y se descarta el resultado.
    hayPartida_ = GestorGuardado::existePartida(GestorGuardado::rutaPorDefecto())
               && GestorGuardado::cargar(GestorGuardado::rutaPorDefecto()) != nullptr;

    // El nombre arranca vacio: lo pone el jugador, no el juego.
    nombre_.clear();

    construirInterfaz();
    refrescarPrevia();
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
    etiquetaEspecie_.setFillColor(tema::kTextoSuave);
    etiquetaGenero_.setFillColor(tema::kTextoSuave);
    etiquetaNombre_.setFillColor(tema::kTextoSuave);
    descripcion_.setFillColor(tema::kTextoSuave);
    ayuda_.setFillColor(tema::kTextoTenue);

    centrar(titulo_,    30.f);
    centrar(subtitulo_, 74.f);

    // --- Dos grupos de botones, uno a cada lado del centro -------------------
    // Especie a la izquierda, genero a la derecha. Cada grupo son dos botones,
    // asi que los dos ocupan lo mismo y quedan simetricos.
    const auto tipos = FabricaMascotas::tiposDisponibles();

    const float anchoGrupo = 2.f * kAnchoBoton + kHueco;
    const float separacion = 100.f;                   // pasillo entre los grupos
    const float xEspecies  = tamanoVentana_.x * 0.5f - separacion * 0.5f - anchoGrupo;
    const float xGeneros   = tamanoVentana_.x * 0.5f + separacion * 0.5f;

    etiquetaEspecie_.setPosition({ xEspecies, kYBotones - 20.f });
    etiquetaGenero_.setPosition ({ xGeneros,  kYBotones - 20.f });

    const sf::Vector2f tamanoBoton(kAnchoBoton, kAltoBoton);

    inicioEspecies_ = botones_.size();
    float x = xEspecies;
    for (TipoMascota tipo : tipos)
    {
        botones_.emplace_back(fuente_, FabricaMascotas::nombreTipo(tipo),
                              sf::Vector2f(x, kYBotones), tamanoBoton,
                              [this, tipo] { elegirEspecie(tipo); });
        x += kAnchoBoton + kHueco;
    }

    inicioGeneros_ = botones_.size();
    x = xGeneros;
    for (Genero genero : { Genero::Macho, Genero::Hembra })
    {
        botones_.emplace_back(fuente_, nombreGenero(genero),
                              sf::Vector2f(x, kYBotones), tamanoBoton,
                              [this, genero] { elegirGenero(genero); });
        x += kAnchoBoton + kHueco;
    }

    // --- Recuadro donde se ve la mascota elegida -----------------------------
    const sf::Vector2f tamanoPanel(320.f, 206.f);
    panelPrevio_.setSize(tamanoPanel);
    panelPrevio_.setPosition({ (tamanoVentana_.x - tamanoPanel.x) * 0.5f, 186.f });
    panelPrevio_.setFillColor(tema::kPanel);
    panelPrevio_.setOutlineThickness(1.f);
    panelPrevio_.setOutlineColor(tema::kPanelBorde);

    vista_.establecerEscala(3.f);   // entero, igual que en la partida
    vista_.establecerPosicion({ tamanoVentana_.x * 0.5f, 186.f + tamanoPanel.y * 0.55f });

    // --- Campo de texto del nombre ------------------------------------------
    const sf::Vector2f tamanoCampo(320.f, 46.f);
    campoNombre_.setSize(tamanoCampo);
    campoNombre_.setPosition({ (tamanoVentana_.x - tamanoCampo.x) * 0.5f, 470.f });
    campoNombre_.setFillColor(tema::kFondo);
    campoNombre_.setOutlineThickness(1.f);
    campoNombre_.setOutlineColor(tema::kAcento);

    etiquetaNombre_.setPosition({ campoNombre_.getPosition().x, 450.f });

    // --- Comenzar y continuar ------------------------------------------------
    const sf::Vector2f tamanoAccion(200.f, 48.f);
    indiceComenzar_ = botones_.size();
    botones_.emplace_back(fuente_, "Comenzar",
                          sf::Vector2f((tamanoVentana_.x - tamanoAccion.x) * 0.5f, 532.f),
                          tamanoAccion, [this] { confirmar(); });
    botones_.back().establecerColores(tema::kAcento, tema::kBotonHover);

    if (hayPartida_)
    {
        botones_.emplace_back(fuente_, "Continuar partida",
                              sf::Vector2f(tamanoVentana_.x - 240.f, 30.f),
                              sf::Vector2f(200.f, 40.f),
                              [this] { continuarPartida(); });
        botones_.back().establecerTamanoTexto(tema::kTextoChico);
    }

    centrar(ayuda_, tamanoVentana_.y - 34.f);
    refrescarComenzar();
}

// --------------------------------------------------------------- Acciones ---

std::string PantallaSeleccion::nombreParaMostrar() const
{
    return nombre_.empty() ? kNombrePrevio : nombre_;
}

void PantallaSeleccion::elegirEspecie(TipoMascota tipo)
{
    tipoElegido_ = tipo;
    refrescarPrevia();
}

void PantallaSeleccion::elegirGenero(Genero genero)
{
    generoElegido_ = genero;
    refrescarPrevia();
}

void PantallaSeleccion::refrescarPrevia()
{
    mascota_ = FabricaMascotas::crear(tipoElegido_, generoElegido_, nombreParaMostrar());
    if (!mascota_) return;

    vista_.prepararMascota(*mascota_);
    descripcion_.setString(mascota_->descripcion());

    // La descripcion se parte en dos lineas si no cabe en el ancho del panel.
    if (descripcion_.getLocalBounds().size.x > tamanoVentana_.x - 200.f)
    {
        std::string texto = mascota_->descripcion();
        const std::size_t mitad = texto.find(' ', texto.size() / 2);
        if (mitad != std::string::npos) texto[mitad] = '\n';
        descripcion_.setString(texto);
    }
    centrar(descripcion_, 404.f);

    // Se resalta el boton elegido de cada grupo.
    const auto tipos = FabricaMascotas::tiposDisponibles();
    for (std::size_t i = 0; i < tipos.size(); ++i)
        botones_[inicioEspecies_ + i].establecerActivo(tipos[i] == tipoElegido_);

    botones_[inicioGeneros_ + 0].establecerActivo(generoElegido_ == Genero::Macho);
    botones_[inicioGeneros_ + 1].establecerActivo(generoElegido_ == Genero::Hembra);
}

void PantallaSeleccion::refrescarComenzar()
{
    // Sin nombre no se empieza: es mas claro apagar el boton que dejar que se
    // pulse y poner uno por defecto a espaldas del jugador.
    botones_[indiceComenzar_].establecerHabilitado(!nombre_.empty());
}

void PantallaSeleccion::confirmar()
{
    if (nombre_.empty()) return;

    // La mascota de la vista previa se ha ido creando y renombrando mientras
    // el jugador probaba especies, asi que su bitacora arrastra mensajes con
    // el nombre de relleno. La que se entrega a la partida es nueva, con el
    // nombre definitivo desde el principio.
    auto definitiva = FabricaMascotas::crear(tipoElegido_, generoElegido_, nombre_);
    if (!definitiva) return;

    mascota_ = std::move(definitiva);
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

    if (mascota_)
    {
        mascota_->establecerNombre(nombreParaMostrar());
        descripcion_.setString(mascota_->descripcion());
        centrar(descripcion_, 404.f);
    }
    refrescarComenzar();
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
        // Las flechas no generan TextEntered, asi que se pueden usar para
        // navegar sin estorbar a quien esta escribiendo el nombre.
        using Tecla = sf::Keyboard::Key;
        const auto tipos = FabricaMascotas::tiposDisponibles();

        switch (tecla->code)
        {
            case Tecla::Escape: solicitar(Transicion::Menu); break;

            case Tecla::Left:
            case Tecla::Right:
            {
                const int cuantas = static_cast<int>(tipos.size());
                int indice = 0;
                for (int i = 0; i < cuantas; ++i)
                    if (tipos[static_cast<std::size_t>(i)] == tipoElegido_) indice = i;

                const int paso = (tecla->code == Tecla::Left) ? -1 : 1;
                elegirEspecie(tipos[static_cast<std::size_t>((indice + paso + cuantas) % cuantas)]);
                break;
            }

            case Tecla::Up:
            case Tecla::Down:
                elegirGenero(generoElegido_ == Genero::Macho ? Genero::Hembra : Genero::Macho);
                break;

            default: break;
        }
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

    const bool cursorVisible = relojCursor_ < 0.5f;

    // Con el campo vacio se muestra una pista en gris, no un nombre puesto por
    // el juego: asi queda claro que hay que escribir algo.
    if (nombre_.empty())
    {
        textoNombre_.setString(std::string(cursorVisible ? "|" : " ") + " escribe aqui");
        textoNombre_.setFillColor(tema::kTextoTenue);
    }
    else
    {
        textoNombre_.setString(nombre_ + (cursorVisible ? "|" : " "));
        textoNombre_.setFillColor(tema::kTexto);
    }

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

    objetivo.draw(etiquetaEspecie_);
    objetivo.draw(etiquetaGenero_);

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
