#include "PantallaMenu.hpp"

#include "Mosaico.hpp"
#include "Tema.hpp"

#include <cmath>

namespace vp {

namespace {

const sf::Color kAmarillo(255, 220, 80);    // el mismo tono del titulo de Proyecto3P
const sf::Color kFondoBase(38, 26, 54);     // morado oscuro, el del menu original

constexpr unsigned kTamTitulo  = 56;
constexpr unsigned kTamOpcion  = 40;
constexpr float    kSeparacion = 30.f;

} // namespace sin nombre

PantallaMenu::PantallaMenu(const sf::Font& fuente, sf::Vector2f tamanoVentana)
    : fuente_(fuente)
    , tamanoVentana_(tamanoVentana)
    , titulo_(fuente, "VIRTUALPET", kTamTitulo)
    , pie_(fuente, "FLECHAS MOVER    ENTER ELEGIR    M MUSICA", tema::kTextoChico)
{
    construirFondo();

    // --- Marquesina: el marco iluminado del titulo de una recreativa --------
    const sf::Vector2f tamanoMarco(600.f, 96.f);
    const sf::Vector2f posMarco((tamanoVentana_.x - tamanoMarco.x) / 2.f, 46.f);
    marquesina_ = tema::panelBiselado(posMarco, tamanoMarco, tema::kPanelBorde);

    // El titulo sigue inclinado, como en el menu del Proyecto3P.
    titulo_.setFillColor(kAmarillo);
    const sf::FloatRect limites = titulo_.getLocalBounds();
    titulo_.setOrigin({ limites.position.x + limites.size.x / 2.f,
                        limites.position.y + limites.size.y / 2.f });
    titulo_.setPosition({ tamanoVentana_.x / 2.f, posMarco.y + tamanoMarco.y / 2.f });
    titulo_.setRotation(sf::degrees(-8.f));

    // --- Opciones ------------------------------------------------------------
    const std::vector<std::string> opciones { "Jugar", "Opciones", "Salir" };
    textosMenu_.reserve(opciones.size());
    placas_.reserve(opciones.size());

    for (const std::string& texto : opciones)
    {
        textosMenu_.emplace_back(fuente_, texto, kTamOpcion);
        placas_.emplace_back();
    }

    colocarOpciones();

    // Triangulo senalador, a la izquierda de la placa elegida.
    senalador_.setPointCount(3);
    senalador_.setPoint(0, { 0.f,  0.f });
    senalador_.setPoint(1, { 16.f, 9.f });
    senalador_.setPoint(2, { 0.f, 18.f });
    senalador_.setFillColor(kAmarillo);

    // --- Tira inferior con la ayuda, igual que la botonera de la partida -----
    const float altoTira = 34.f;
    tiraPie_ = tema::panelBiselado({ 0.f, tamanoVentana_.y - altoTira },
                                   { tamanoVentana_.x, altoTira });

    pie_.setFillColor(tema::kTextoSuave);
    const sf::FloatRect limitesPie = pie_.getLocalBounds();
    pie_.setOrigin({ limitesPie.position.x + limitesPie.size.x / 2.f, 0.f });
    pie_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y - altoTira + 10.f });

    barrido_ = tema::barridoCRT(tamanoVentana_);

    // --- Musica e icono de altavoz -------------------------------------------
    if (musica_.load("assets/sound/Menu.ogg"))
    {
        musica_.setLoop(true);
        musica_.setVolume(45.f);
        musica_.play();
    }
    // La imagen del altavoz es enorme (1511x1306): a 0.03 queda como un icono.
    botonMusica_.cargar("assets/images/speaker.png", { 26.f, 22.f }, 0.03f);
    botonMusica_.establecerActivo(musica_.isPlaying());
}

void PantallaMenu::construirFondo()
{
    // El fondo del menu de Proyecto3P es un patron repetido del personaje.
    // Aqui se hace lo mismo, pero componiendolo en caliente a partir de las
    // hojas de sprites, para no depender de una imagen dibujada aparte. Se
    // alternan las dos especies, que es lo que ofrece el juego.
    if (!componerMosaico(lienzoFondo_, tamanoVentana_,
                         { "assets/images/conejo_macho.txt",
                           "assets/images/castor_macho.txt" },
                         kFondoBase))
        return;

    fondo_.emplace(lienzoFondo_.getTexture());

    // Un velo oscuro encima, para que el texto se lea sobre el patron.
    velo_.setSize(tamanoVentana_);
    velo_.setFillColor(sf::Color(20, 12, 30, 90));
}

void PantallaMenu::colocarOpciones()
{
    const float altoTotal = placas_.size() * kAltoPlaca +
                            (placas_.size() - 1) * kHuecoPlaca;
    const float inicioY   = (tamanoVentana_.y - altoTotal) / 2.f + 24.f;
    const float x         = (tamanoVentana_.x - kAnchoPlaca) / 2.f;

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        const float y = inicioY + i * (kAltoPlaca + kHuecoPlaca);

        placas_[i] = tema::paralelogramo({ x, y }, { kAnchoPlaca, kAltoPlaca });

        sf::Text& texto = textosMenu_[i];
        const sf::FloatRect limites = texto.getLocalBounds();
        texto.setOrigin({ limites.position.x + limites.size.x / 2.f,
                          limites.position.y + limites.size.y / 2.f });
        texto.setPosition({ tamanoVentana_.x / 2.f, y + kAltoPlaca / 2.f });
    }
}

void PantallaMenu::mover(int direccion)
{
    const int total = static_cast<int>(textosMenu_.size());
    seleccion_ = static_cast<std::size_t>((static_cast<int>(seleccion_) + direccion + total) % total);
}

void PantallaMenu::elegir(std::size_t indice)
{
    switch (static_cast<OpcionMenu>(indice))
    {
        case OpcionMenu::Jugar:    solicitar(Transicion::Seleccion); break;
        case OpcionMenu::Opciones: solicitar(Transicion::Opciones);  break;
        case OpcionMenu::Salir:    solicitar(Transicion::Salir);     break;
    }
}

void PantallaMenu::manejarEvento(const sf::Event& evento)
{
    if (evento.is<sf::Event::Closed>())
    {
        solicitar(Transicion::Salir);
        return;
    }

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {
        using Tecla = sf::Keyboard::Key;
        switch (tecla->code)
        {
            case Tecla::Up:
            case Tecla::W:      mover(-1); break;

            case Tecla::Down:
            case Tecla::S:      mover(+1); break;

            case Tecla::Enter:
            case Tecla::Space:  elegir(seleccion_); break;

            case Tecla::Escape: solicitar(Transicion::Salir); break;

            case Tecla::M:
            {
                // Igual que en Proyecto3P: M silencia y reactiva la musica.
                const bool sonando = musica_.isPlaying();
                if (sonando) musica_.pause(); else musica_.play();
                botonMusica_.establecerActivo(!sonando);
                break;
            }

            default: break;
        }
        return;
    }

    if (const auto* movimiento = evento.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2f punto(movimiento->position);

        // Solo cuenta si el raton se ha movido de verdad. Al traer la ventana
        // al frente, Windows manda un aviso de movimiento con el cursor
        // quieto, y eso le robaba la seleccion al teclado sin que nadie
        // hubiera tocado el raton.
        const bool seMovio = (punto != raton_);
        raton_ = punto;
        if (!seMovio) return;

        // Pasar el raton por encima de una placa la selecciona. Se prueba
        // contra la placa y no contra el texto: el area es mayor y mas
        // predecible que las letras sueltas.
        for (std::size_t i = 0; i < placas_.size(); ++i)
            if (placas_[i].getGlobalBounds().contains(raton_))
                seleccion_ = i;
        return;
    }

    if (const auto* clic = evento.getIf<sf::Event::MouseButtonPressed>())
    {
        if (clic->button != sf::Mouse::Button::Left) return;

        const sf::Vector2f punto(clic->position);

        if (botonMusica_.contiene(punto))
        {
            const bool sonando = musica_.isPlaying();
            if (sonando) musica_.pause(); else musica_.play();
            botonMusica_.establecerActivo(!sonando);
            return;
        }

        for (std::size_t i = 0; i < placas_.size(); ++i)
        {
            if (placas_[i].getGlobalBounds().contains(punto))
            {
                elegir(i);
                return;
            }
        }
    }
}

void PantallaMenu::actualizar(float dt)
{
    reloj_ += dt;

    const float altoTotal = placas_.size() * kAltoPlaca +
                            (placas_.size() - 1) * kHuecoPlaca;
    const float inicioY   = (tamanoVentana_.y - altoTotal) / 2.f + 24.f;
    const float x         = (tamanoVentana_.x - kAnchoPlaca) / 2.f;

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        const bool elegida = (i == seleccion_);

        // La placa elegida se enciende en amarillo y el texto pasa a oscuro,
        // como una tecla retroiluminada. Las demas quedan apagadas.
        placas_[i].setFillColor(elegida ? kAmarillo : tema::kBoton);
        placas_[i].setOutlineThickness(2.f);
        placas_[i].setOutlineColor(elegida ? tema::kTexto : tema::kPanelBorde);

        textosMenu_[i].setFillColor(elegida ? tema::kFondo : tema::kTexto);

        // Y late un poco, para que se vea cual es sin depender solo del color.
        const float escala = elegida ? 1.f + std::sin(reloj_ * 4.f) * 0.03f : 1.f;
        textosMenu_[i].setScale({ escala, escala });
    }

    // El senalador acompana a la placa activa, con un vaiven corto.
    const float y = inicioY + seleccion_ * (kAltoPlaca + kHuecoPlaca);
    const float empuje = std::sin(reloj_ * 5.f) * 4.f;
    senalador_.setPosition({ x - 34.f + empuje, y + kAltoPlaca / 2.f - 9.f });
}

void PantallaMenu::dibujar(sf::RenderTarget& objetivo) const
{
    if (fondo_) objetivo.draw(*fondo_);
    objetivo.draw(velo_);

    marquesina_.dibujar(objetivo);
    tema::dibujarConSombra(objetivo, titulo_, 3.f);

    for (std::size_t i = 0; i < placas_.size(); ++i)
    {
        objetivo.draw(placas_[i]);
        tema::dibujarConSombra(objetivo, textosMenu_[i]);
    }

    objetivo.draw(senalador_);

    tiraPie_.dibujar(objetivo);
    objetivo.draw(pie_);

    objetivo.draw(botonMusica_);

    // El barrido de tubo va el ultimo, encima de todo lo demas.
    objetivo.draw(barrido_);
}

} // namespace vp
