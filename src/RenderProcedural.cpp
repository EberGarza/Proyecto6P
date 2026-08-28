#include "RenderProcedural.hpp"

#include "Tema.hpp"
#include "Mascota.hpp"

#include <cmath>
#include <cstdint>

namespace vp {
namespace {

/// Mezcla dos colores. t = 0 devuelve a, t = 1 devuelve b.
sf::Color mezclar(sf::Color a, sf::Color b, float t)
{
    const auto lerp = [t](std::uint8_t x, std::uint8_t y)
    {
        return static_cast<std::uint8_t>(x + (y - x) * t);
    };
    return sf::Color(lerp(a.r, b.r), lerp(a.g, b.g), lerp(a.b, b.b));
}

/// Centra el origen de un circulo en su propio centro geometrico.
void centrar(sf::CircleShape& figura, float radio)
{
    figura.setRadius(radio);
    figura.setOrigin({ radio, radio });
}

} // namespace sin nombre

RenderProcedural::RenderProcedural(const sf::Font& fuente)
    : boca_(sf::PrimitiveType::LineStrip, kPuntosBoca)
    , efecto_(fuente, "", tema::kTextoTitulo)
{
    centrar(cuerpo_,     kRadioCuerpo);
    centrar(panza_,      kRadioPanza);
    centrar(orejaIzq_,   kRadioOreja);
    centrar(orejaDer_,   kRadioOreja);
    centrar(ojoIzq_,     kRadioOjo);
    centrar(ojoDer_,     kRadioOjo);
    centrar(pupilaIzq_,  kRadioPupila);
    centrar(pupilaDer_,  kRadioPupila);
    centrar(hocico_,     kRadioHocico);

    cuerpo_.setOutlineThickness(3.f);
    cuerpo_.setOutlineColor(sf::Color(0, 0, 0, 60));

    ojoIzq_.setFillColor(sf::Color::White);
    ojoDer_.setFillColor(sf::Color::White);
    pupilaIzq_.setFillColor(sf::Color(28, 28, 36));
    pupilaDer_.setFillColor(sf::Color(28, 28, 36));

    parpadoIzq_.setSize({ kRadioOjo * 2.f, kRadioOjo * 2.f });
    parpadoDer_.setSize({ kRadioOjo * 2.f, kRadioOjo * 2.f });

    cola_.setSize({ 46.f, 12.f });
    cola_.setOrigin({ 0.f, 6.f });

    efecto_.setFillColor(tema::kTexto);

    configurarEspecie("Perro");
    aplicarEstado(TipoEstado::Normal);
    establecerEscala(1.f);
}

// ------------------------------------------------------------- Especies -----

void RenderProcedural::configurarEspecie(const std::string& especie)
{
    if (especie == especieActual_) return;
    especieActual_ = especie;

    if (especie == "Gastly")
    {
        colorCuerpo_ = sf::Color(150, 110, 168);
        colorPanza_  = sf::Color(196, 158, 208);
        orejaIzq_.setPointCount(3);          // mechones de gas
        orejaDer_.setPointCount(3);
    }
    else if (especie == "Gato")
    {
        colorCuerpo_ = sf::Color(148, 152, 168);
        colorPanza_  = sf::Color(216, 220, 232);
        orejaIzq_.setPointCount(3);          // orejas puntiagudas
        orejaDer_.setPointCount(3);
    }
    else if (especie == "Dragon")
    {
        colorCuerpo_ = sf::Color(96, 168, 110);
        colorPanza_  = sf::Color(196, 226, 168);
        orejaIzq_.setPointCount(3);          // cuernos
        orejaDer_.setPointCount(3);
    }
    else                                      // Perro y cualquier especie nueva
    {
        colorCuerpo_ = sf::Color(190, 150, 105);
        colorPanza_  = sf::Color(232, 208, 176);
        orejaIzq_.setPointCount(24);         // orejas redondas
        orejaDer_.setPointCount(24);
    }

    panza_.setFillColor(colorPanza_);
    hocico_.setFillColor(mezclar(colorPanza_, sf::Color::Black, 0.25f));

    // El color del cuerpo se calcula en aplicarEstado(), asi que hay que
    // recalcularlo: si no, al cambiar de especie se quedaria el color viejo.
    aplicarEstado(estado_);
}

// -------------------------------------------------------------- Estados -----

void RenderProcedural::aplicarEstado(TipoEstado tipo)
{
    estado_ = tipo;

    // Valores por defecto que luego ajusta cada caso.
    tinteEstado_   = sf::Color::White;
    aperturaOjos_  = 1.f;
    curvaturaBoca_ = 0.35f;
    velocidadBote_ = 2.0f;
    alturaBote_    = 4.f;
    efecto_.setString("");
    efecto_.setFillColor(tema::kTexto);

    switch (tipo)
    {
        case TipoEstado::Normal:
            break;

        case TipoEstado::Feliz:
            curvaturaBoca_ = 1.0f;
            velocidadBote_ = 5.0f;
            alturaBote_    = 11.f;
            efecto_.setString("<3");
            efecto_.setFillColor(tema::kMal);
            break;

        case TipoEstado::Hambrienta:
            curvaturaBoca_ = -0.7f;
            velocidadBote_ = 1.4f;
            tinteEstado_   = sf::Color(235, 225, 205);
            efecto_.setString("...");
            efecto_.setFillColor(tema::kMedio);
            break;

        case TipoEstado::Cansada:
            curvaturaBoca_ = -0.4f;
            aperturaOjos_  = 0.45f;
            velocidadBote_ = 0.9f;
            alturaBote_    = 2.f;
            efecto_.setString("~");
            efecto_.setFillColor(tema::kTextoSuave);
            break;

        case TipoEstado::Durmiendo:
            curvaturaBoca_ = 0.15f;
            aperturaOjos_  = 0.f;
            velocidadBote_ = 0.7f;
            alturaBote_    = 6.f;
            efecto_.setString("Z z z");
            efecto_.setFillColor(tema::kAcento);
            break;

        case TipoEstado::Enferma:
            curvaturaBoca_ = -1.0f;
            aperturaOjos_  = 0.55f;
            velocidadBote_ = 1.1f;
            tinteEstado_   = sf::Color(196, 226, 186);   // verdoso
            efecto_.setString("+");
            efecto_.setFillColor(tema::kBien);
            break;

        case TipoEstado::Jugando:
            curvaturaBoca_ = 1.0f;
            velocidadBote_ = 8.0f;
            alturaBote_    = 16.f;
            efecto_.setString("!");
            efecto_.setFillColor(tema::kAcento);
            break;

        case TipoEstado::Muerta:
            curvaturaBoca_ = -0.9f;
            aperturaOjos_  = 0.f;
            velocidadBote_ = 0.f;
            alturaBote_    = 0.f;
            tinteEstado_   = sf::Color(120, 120, 130);   // gris
            efecto_.setString("R.I.P.");
            efecto_.setFillColor(tema::kTextoTenue);
            break;
    }

    cuerpo_.setFillColor(mezclar(colorCuerpo_, tinteEstado_, 0.45f));
    orejaIzq_.setFillColor(mezclar(colorCuerpo_, sf::Color::Black, 0.15f));
    orejaDer_.setFillColor(orejaIzq_.getFillColor());
    cola_.setFillColor(orejaIzq_.getFillColor());
}

// ---------------------------------------------------------- Actualizacion ---

void RenderProcedural::actualizar(const Mascota& mascota, float dt)
{
    tiempo_ += dt;

    configurarEspecie(mascota.especie());

    if (mascota.tipoEstado() != estado_)
        aplicarEstado(mascota.tipoEstado());

    // Parpadeo: cada pocos segundos los ojos se cierran un instante.
    if (estado_ != TipoEstado::Durmiendo && estado_ != TipoEstado::Muerta)
    {
        relojParpadeo_ += dt;
        if (relojParpadeo_ > 3.2f) relojParpadeo_ = 0.f;
    }

    // La cola se mueve mas rapido cuanto mas feliz esta la mascota.
    giroCola_ = std::sin(tiempo_ * (2.f + mascota.felicidad().porcentaje() * 8.f)) * 22.f;

    const float rebote = std::sin(tiempo_ * velocidadBote_) * alturaBote_;
    reposicionar(rebote * escala_);
}

void RenderProcedural::reposicionar(float desplazamientoY)
{
    const float  e = escala_;
    const sf::Vector2f c(posicion_.x, posicion_.y + desplazamientoY);

    cuerpo_.setPosition(c);
    panza_.setPosition ({ c.x,             c.y + 18.f * e });
    hocico_.setPosition({ c.x,              c.y + 16.f * e });

    orejaIzq_.setPosition({ c.x - 42.f * e, c.y - 46.f * e });
    orejaDer_.setPosition({ c.x + 42.f * e, c.y - 46.f * e });

    const sf::Vector2f centroOjoIzq(c.x - 22.f * e, c.y - 12.f * e);
    const sf::Vector2f centroOjoDer(c.x + 22.f * e, c.y - 12.f * e);
    ojoIzq_.setPosition(centroOjoIzq);
    ojoDer_.setPosition(centroOjoDer);

    // Las pupilas miran ligeramente de lado, siguiendo el vaiven del cuerpo.
    const float mirada = std::sin(tiempo_ * 0.8f) * 4.f * e;
    pupilaIzq_.setPosition({ centroOjoIzq.x + mirada, centroOjoIzq.y + 2.f * e });
    pupilaDer_.setPosition({ centroOjoDer.x + mirada, centroOjoDer.y + 2.f * e });

    // Parpado: un rectangulo del color del cuerpo que baja sobre el ojo.
    // Se combina la apertura del estado con el parpadeo automatico.
    const bool  parpadeando = relojParpadeo_ > 3.05f;
    const float apertura    = parpadeando ? 0.f : aperturaOjos_;
    const float altoParpado = kRadioOjo * 2.f * e * (1.f - apertura);

    const sf::Color colorParpado = cuerpo_.getFillColor();
    for (sf::RectangleShape* parpado : { &parpadoIzq_, &parpadoDer_ })
    {
        parpado->setSize({ kRadioOjo * 2.2f * e, altoParpado });
        parpado->setFillColor(colorParpado);
    }
    parpadoIzq_.setPosition({ centroOjoIzq.x - kRadioOjo * 1.1f * e,
                              centroOjoIzq.y - kRadioOjo * e });
    parpadoDer_.setPosition({ centroOjoDer.x - kRadioOjo * 1.1f * e,
                              centroOjoDer.y - kRadioOjo * e });

    cola_.setSize({ 46.f * e, 12.f * e });
    cola_.setOrigin({ 0.f, 6.f * e });
    cola_.setPosition({ c.x + 52.f * e, c.y + 26.f * e });
    // SFML 3 usa el tipo sf::Angle en vez de un float suelto, para que no haya
    // duda de si son grados o radianes.
    cola_.setRotation(sf::degrees(giroCola_));

    construirBoca({ c.x, c.y + 30.f * e }, curvaturaBoca_);

    efecto_.setCharacterSize(static_cast<unsigned>(tema::kTextoTitulo * e));
    efecto_.setPosition({ c.x + 62.f * e, c.y - 84.f * e });
}

void RenderProcedural::construirBoca(sf::Vector2f centro, float curvatura)
{
    const float ancho  = 40.f * escala_;
    const float altura = 14.f * escala_ * curvatura;
    const sf::Color color(48, 40, 44);

    for (int i = 0; i < kPuntosBoca; ++i)
    {
        // t va de -1 a 1; la parabola 1 - t^2 da la curva de la sonrisa.
        const float t = -1.f + 2.f * static_cast<float>(i) / (kPuntosBoca - 1);
        const float x = centro.x + t * ancho * 0.5f;
        const float y = centro.y - (1.f - t * t) * altura;

        boca_[static_cast<std::size_t>(i)].position = { x, y };
        boca_[static_cast<std::size_t>(i)].color    = color;
    }
}

// ------------------------------------------------------ Posicion y escala ---

void RenderProcedural::establecerPosicion(sf::Vector2f posicion)
{
    posicion_ = posicion;
    reposicionar(0.f);
}

void RenderProcedural::establecerEscala(float escala)
{
    escala_ = (escala > 0.f) ? escala : 1.f;

    centrar(cuerpo_,    kRadioCuerpo * escala_);
    centrar(panza_,     kRadioPanza  * escala_);
    centrar(orejaIzq_,  kRadioOreja  * escala_);
    centrar(orejaDer_,  kRadioOreja  * escala_);
    centrar(ojoIzq_,    kRadioOjo    * escala_);
    centrar(ojoDer_,    kRadioOjo    * escala_);
    centrar(pupilaIzq_, kRadioPupila * escala_);
    centrar(pupilaDer_, kRadioPupila * escala_);
    centrar(hocico_,    kRadioHocico * escala_);

    reposicionar(0.f);
}

// --------------------------------------------------------------- Dibujado ---

void RenderProcedural::draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const
{
    // El orden importa: de atras hacia adelante.
    objetivo.draw(cola_,       estados);
    objetivo.draw(orejaIzq_,   estados);
    objetivo.draw(orejaDer_,   estados);
    objetivo.draw(cuerpo_,     estados);
    objetivo.draw(panza_,      estados);
    objetivo.draw(ojoIzq_,     estados);
    objetivo.draw(ojoDer_,     estados);
    objetivo.draw(pupilaIzq_,  estados);
    objetivo.draw(pupilaDer_,  estados);
    objetivo.draw(parpadoIzq_, estados);
    objetivo.draw(parpadoDer_, estados);
    objetivo.draw(hocico_,     estados);
    objetivo.draw(boca_,       estados);
    objetivo.draw(efecto_,     estados);
}

} // namespace vp
