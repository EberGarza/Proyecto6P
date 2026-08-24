#include "PantallaMenu.hpp"

#include "HojaSprites.hpp"
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
    , pie_(fuente, "Flechas o W/S para moverte   -   Enter para elegir", 16)
{
    construirFondo();

    // --- Titulo, inclinado como en el menu del Proyecto3P ---
    titulo_.setFillColor(kAmarillo);
    const sf::FloatRect limites = titulo_.getLocalBounds();
    titulo_.setOrigin({ limites.position.x + limites.size.x / 2.f,
                        limites.position.y + limites.size.y / 2.f });
    titulo_.setPosition({ tamanoVentana_.x / 2.f, 100.f });
    titulo_.setRotation(sf::degrees(-8.f));

    // --- Opciones ---
    const std::vector<std::string> opciones { "Jugar", "Opciones", "Salir" };
    textosMenu_.reserve(opciones.size());
    for (const std::string& texto : opciones)
        textosMenu_.emplace_back(fuente_, texto, kTamOpcion);

    colocarOpciones();

    // --- Pie de ayuda ---
    pie_.setFillColor(sf::Color(255, 255, 255, 130));
    const sf::FloatRect limitesPie = pie_.getLocalBounds();
    pie_.setOrigin({ limitesPie.position.x + limitesPie.size.x / 2.f, 0.f });
    pie_.setPosition({ tamanoVentana_.x / 2.f, tamanoVentana_.y - 46.f });

    // --- Musica e icono de altavoz ---
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
    const auto ancho = static_cast<unsigned>(tamanoVentana_.x);
    const auto alto  = static_cast<unsigned>(tamanoVentana_.y);

    if (!lienzoFondo_.resize({ ancho, alto })) return;

    lienzoFondo_.clear(kFondoBase);

    // El fondo del menu de Proyecto3P es un patron repetido del personaje. Aqui
    // se hace lo mismo, pero componiendolo en caliente a partir de las propias
    // hojas de sprites, para no depender de una imagen de fondo dibujada
    // aparte. Se alternan las dos especies, que es lo que ofrece el juego.
    //
    // HojaSprites no se puede copiar ni mover (cada Animacion guarda un puntero
    // a su textura), asi que las dos viven como variables sueltas y se eligen
    // con un indice, en vez de meterlas en un vector.
    HojaSprites hojaConejo;
    HojaSprites hojaCastor;

    const bool hayConejo = hojaConejo.cargar("assets/images/conejo_macho.txt");
    const bool hayCastor = hojaCastor.cargar("assets/images/castor_macho.txt");

    if (hayConejo || hayCastor)
    {
        // Un sprite por hoja, ya colocado en su primer cuadro de reposo.
        std::optional<sf::Sprite> sellos[2];

        const auto preparar = [](std::optional<sf::Sprite>& destino,
                                 const HojaSprites& hoja, bool disponible)
        {
            if (!disponible) return;
            const Animacion* idle = hoja.animacion(TipoEstado::Normal);
            if (!idle) return;

            destino.emplace(hoja.textura());
            idle->aplicarCuadroActual(*destino);
            destino->setColor(sf::Color(255, 255, 255, 26));   // muy tenue
            destino->setScale({ 2.f, 2.f });
        };

        preparar(sellos[0], hojaConejo, hayConejo);
        preparar(sellos[1], hojaCastor, hayCastor);

        const float paso = 128.f;
        int fila = 0;
        for (float y = 20.f; y < tamanoVentana_.y + paso; y += paso, ++fila)
        {
            // Filas alternas desplazadas, para que no se vea una rejilla.
            const float desfase = (fila % 2 == 0) ? 0.f : paso / 2.f;
            int columna = 0;

            for (float x = 20.f + desfase; x < tamanoVentana_.x + paso; x += paso, ++columna)
            {
                // Tablero de ajedrez entre las dos especies. Si solo hay una
                // hoja cargada, se usa esa en todas las casillas.
                auto& sello = sellos[(fila + columna) % 2];
                auto& usado = sello ? sello : (sellos[0] ? sellos[0] : sellos[1]);
                if (!usado) continue;

                usado->setRotation(sf::degrees(((fila + columna) % 5) * 7.f - 14.f));
                usado->setPosition({ x, y });
                lienzoFondo_.draw(*usado);
            }
        }
    }

    // Un velo oscuro en el centro para que el texto se lea sobre el patron.
    sf::RectangleShape velo({ tamanoVentana_.x, tamanoVentana_.y });
    velo.setFillColor(sf::Color(20, 12, 30, 90));
    lienzoFondo_.draw(velo);

    lienzoFondo_.display();
    fondo_.emplace(lienzoFondo_.getTexture());
}

void PantallaMenu::colocarOpciones()
{
    const float altoTotal = textosMenu_.size() * kTamOpcion +
                            (textosMenu_.size() - 1) * kSeparacion;
    const float inicioY = (tamanoVentana_.y - altoTotal) / 2.f;

    for (std::size_t i = 0; i < textosMenu_.size(); ++i)
    {
        sf::Text& texto = textosMenu_[i];
        const sf::FloatRect limites = texto.getLocalBounds();
        texto.setOrigin({ limites.position.x + limites.size.x / 2.f, limites.position.y });
        texto.setPosition({ tamanoVentana_.x / 2.f,
                            inicioY + i * (kTamOpcion + kSeparacion) });
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
        raton_ = sf::Vector2f(movimiento->position);

        // Pasar el raton por encima de una opcion la selecciona.
        for (std::size_t i = 0; i < textosMenu_.size(); ++i)
            if (textosMenu_[i].getGlobalBounds().contains(raton_))
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

        for (std::size_t i = 0; i < textosMenu_.size(); ++i)
        {
            if (textosMenu_[i].getGlobalBounds().contains(punto))
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

    for (std::size_t i = 0; i < textosMenu_.size(); ++i)
    {
        const bool elegida = (i == seleccion_);
        textosMenu_[i].setFillColor(elegida ? kAmarillo : sf::Color::White);

        // La opcion activa late un poco, para que se vea cual es sin depender
        // solo del color.
        const float escala = elegida ? 1.f + std::sin(reloj_ * 4.f) * 0.03f : 1.f;
        textosMenu_[i].setScale({ escala, escala });
    }
}

void PantallaMenu::dibujar(sf::RenderTarget& objetivo) const
{
    if (fondo_) objetivo.draw(*fondo_);

    objetivo.draw(titulo_);
    for (const sf::Text& texto : textosMenu_)
        objetivo.draw(texto);

    objetivo.draw(pie_);
    objetivo.draw(botonMusica_);
}

} // namespace vp
