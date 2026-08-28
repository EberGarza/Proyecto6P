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

constexpr float kAltoBotonera         = 110.f;
constexpr float kTiempoConfirmarSalida = 2.5f;

}

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
    , escenaPelea_(fuente, tamanoVentana)
    , textoOcupada_(fuente, "", tema::kTextoNormal)
    , tituloMetrica_(fuente, "TELEMETRIA", tema::kTextoChico)
    , textoMetrica_(fuente, "", tema::kTextoChico)
    , pie_(fuente, "TECLAS 1-7  ACCIONES     ESC  SALIR", 13)
    , anuncio_(fuente, "", 64)
    , tamanoVentana_(tamanoVentana)
{
    construirEscenario();
    construirBarrido();
    construirMetrica();

    const float altoEscenario = tamanoVentana.y - Hud::kAlturaTira - kAltoBotonera;
    centroEscenario_ = { tamanoVentana.x * 0.62f,
                         Hud::kAlturaTira + altoEscenario * 0.46f };

    vista_.prepararMascota(*mascota_);
    vista_.establecerEscala(3.f);
    vista_.establecerPosicion(centroEscenario_);

    panelBotonera_ = tema::panelBiselado(
        { 0.f, tamanoVentana.y - kAltoBotonera },
        { tamanoVentana.x, kAltoBotonera });

    pie_.setFillColor(tema::kTextoTenue);
    pie_.setPosition({ 22.f, tamanoVentana.y - 24.f });

    anuncio_.setFillColor(tema::kAcento);

    panelOcupada_ = tema::panelBiselado(
        { 0.f, tamanoVentana.y - kAltoBotonera - 38.f },
        { tamanoVentana.x, 30.f }, tema::kPanelBorde);
    textoOcupada_.setFillColor(tema::kAcento);

    crearBotones();
    panelAdmin_.enlazar(admin_, *mascota_, inventario_, vista_);
    estadoAnunciado_ = mascota_->tipoEstado();

    if (musica_.load("assets/sound/Game_t.ogg"))
    {
        musica_.setLoop(true);
        musica_.setVolume(40.f);
        musica_.play();
    }
}

void PantallaJuego::construirEscenario()
{

    if (!mascota_) return;

    if (!componerMosaico(lienzoEscenario_, tamanoVentana_,
                         { RenderSprite::rutaDe(mascota_->claveArte()) },
                         tema::kFondo,
                         13,
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

    const float margen = 18.f;
    const float hueco  = 8.f;
    const int   total  = 8;
    const float anchoBoton = (tamanoVentana_.x - margen * 2.f - hueco * (total - 1)) / total;

    const sf::Vector2f tamano(anchoBoton, 52.f);
    float x = margen;
    const float y = tamanoVentana_.y - kAltoBotonera + 16.f;

    Mascota* m = mascota_.get();
    int indiceTecla = 1;

    const auto agregar = [&](const std::string& etiqueta, AccionMascota accion,
                             Boton::Accion hacer, bool conTecla)
    {
        botones_.emplace_back(fuente_, etiqueta, sf::Vector2f(x, y), tamano,
                              std::move(hacer));
        if (conTecla) botones_.back().establecerTecla(std::to_string(indiceTecla++));

        accionDeBoton_.push_back(accion);
        x += tamano.x + hueco;
    };

    agregar("ALIMENTAR", AccionMascota::Alimentar,
            [this] { intentarAccion(AccionMascota::Alimentar); }, true);
    agregar("JUGAR",     AccionMascota::Jugar,
            [this] { intentarAccion(AccionMascota::Jugar); },     true);
    agregar("ASEAR",     AccionMascota::Asear,
            [this] { intentarAccion(AccionMascota::Asear); },     true);
    agregar("MEDICAR",   AccionMascota::Medicar,
            [this] { intentarAccion(AccionMascota::Medicar); },   true);

    agregar("DORMIR",    AccionMascota::Dormir,
            [this] { intentarAccion(AccionMascota::Dormir); },    true);
    agregar("ACARICIAR", AccionMascota::Acariciar,
            [this] { intentarAccion(AccionMascota::Acariciar); }, true);
    agregar("PELEAR",    AccionMascota::Pelear,
            [this] { intentarAccion(AccionMascota::Pelear); },    true);

    if (dynamic_cast<Conejo*>(m))
        agregar("SALTAR", AccionMascota::Especial,
                [this] { intentarAccion(AccionMascota::Especial); }, false);
    else if (dynamic_cast<Castor*>(m))
        agregar("ROER",   AccionMascota::Especial,
                [this] { intentarAccion(AccionMascota::Especial); }, false);
}

void PantallaJuego::intentarAccion(AccionMascota accion)
{
    if (!mascota_) return;

    if (accion == AccionMascota::Dormir &&
        mascota_->tipoEstado() == TipoEstado::Durmiendo)
        accion = AccionMascota::Despertar;

    const Permiso permiso = mascota_->puede(accion);
    if (!permiso)
    {

        mascota_->registrar(permiso.motivo);
        anunciar(permiso.motivo, tema::kMedio);
        return;
    }

    switch (accion)
    {
        case AccionMascota::Alimentar: abrirDespensa();             break;
        case AccionMascota::Jugar:     usarPrimero("Juguete");      break;
        case AccionMascota::Asear:     usarPrimero("Aseo");         break;
        case AccionMascota::Medicar:   usarPrimero("Medicina");     break;
        case AccionMascota::Dormir:    mascota_->dormir();          break;
        case AccionMascota::Despertar: mascota_->despertar();       break;
        case AccionMascota::Acariciar: mascota_->acariciar();       break;
        case AccionMascota::Pelear:    abrirPelea();                break;

        case AccionMascota::Especial:
            if (auto* conejo = dynamic_cast<Conejo*>(mascota_.get())) conejo->saltar();
            else if (auto* castor = dynamic_cast<Castor*>(mascota_.get())) castor->roer();
            break;
    }
}

void PantallaJuego::refrescarBotonera()
{
    for (std::size_t i = 0; i < botones_.size() && i < accionDeBoton_.size(); ++i)
    {
        AccionMascota accion = accionDeBoton_[i];

        if (accion == AccionMascota::Dormir &&
            mascota_->tipoEstado() == TipoEstado::Durmiendo)
            accion = AccionMascota::Despertar;

        botones_[i].establecerDisponible(mascota_->puede(accion).concedido);
    }

    if (!botones_.empty() && botones_.size() > 4)
        botones_[4].establecerEtiqueta(
            mascota_->tipoEstado() == TipoEstado::Durmiendo ? "DESPERTAR" : "DORMIR");

    mostrarOcupada_ = mascota_->estaViva() && mascota_->ocupada();
    if (mostrarOcupada_)
    {
        const std::string que = util::aMinusculas(nombreActividad(mascota_->actividad()));
        textoOcupada_.setString(mascota_->nombre() + " esta " + que + "...");

        const sf::FloatRect limites = textoOcupada_.getLocalBounds();
        textoOcupada_.setOrigin({ limites.position.x + limites.size.x / 2.f, limites.position.y });
        textoOcupada_.setPosition({ tamanoVentana_.x / 2.f,
                                    tamanoVentana_.y - kAltoBotonera - 30.f });
    }
}

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

void PantallaJuego::abrirPelea()
{
    if (!mascota_->estaViva()) return;

    escenaPelea_.abrir(*mascota_, vista_);
}

void PantallaJuego::usarPrimero(const std::string& categoria)
{

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

    fondoProgreso_ = tema::paralelogramo({ pos.x + 12.f, pos.y + tamano.y - 20.f },
                                         { tamano.x - 24.f, 10.f }, 4.f);
    fondoProgreso_.setFillColor(tema::kBarraFondo);
    origenProgreso_ = { pos.x + 12.f, pos.y + tamano.y - 20.f };
    anchoProgreso_  = tamano.x - 24.f;
}

void PantallaJuego::manejarEvento(const sf::Event& evento)
{
    if (evento.is<sf::Event::Closed>())
    {
        solicitar(Transicion::Salir);
        return;
    }

    if (escenaPelea_.visible())
    {
        escenaPelea_.manejarEvento(evento);
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

        if (admin_.visible())      { panelAdmin_.procesarClic(punto);  return; }
        if (panelComida_.visible()) { panelComida_.procesarClic(punto); return; }

        for (Boton& boton : botones_)
            if (boton.procesarClic(punto)) break;
        return;
    }

    if (const auto* escrito = evento.getIf<sf::Event::TextEntered>())
    {

        if (escrito->unicode < 128)
            admin_.registrarTecla(static_cast<char>(escrito->unicode));
        return;
    }

    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>())
    {

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
                if (admin_.visible())
                {
                    admin_.cerrar();
                    break;
                }

                if (confirmandoSalida_)
                {
                    solicitar(Transicion::Salir);
                    break;
                }

                confirmandoSalida_       = true;
                confirmarSalidaRestante_ = kTiempoConfirmarSalida;
                anunciar("Pulsa ESC otra vez para salir", tema::kAcento);
                break;

            case sf::Keyboard::Key::Num1: intentarAccion(AccionMascota::Alimentar); break;
            case sf::Keyboard::Key::Num2: intentarAccion(AccionMascota::Jugar);     break;
            case sf::Keyboard::Key::Num3: intentarAccion(AccionMascota::Asear);     break;
            case sf::Keyboard::Key::Num4: intentarAccion(AccionMascota::Medicar);   break;
            case sf::Keyboard::Key::Num5: intentarAccion(AccionMascota::Dormir);    break;
            case sf::Keyboard::Key::Num6: intentarAccion(AccionMascota::Acariciar); break;
            case sf::Keyboard::Key::Num7: intentarAccion(AccionMascota::Pelear);    break;

            default: break;
        }
    }
}

void PantallaJuego::actualizar(float dt)
{
    fps_ = (dt > 0.f) ? 1.f / dt : 0.f;

    const float dtJuego = dt * admin_.escalaTiempo();

    mascota_->actualizar(dtJuego);
    admin_.aplicarPorTick(*mascota_, dtJuego);

    refrescarBotonera();

    for (Boton& boton : botones_)
    {

        boton.establecerHabilitado(mascota_->estaViva());
        boton.actualizar(raton_);
    }

    vista_.actualizar(*mascota_, dt);
    hud_.actualizar(*mascota_, dt);

    if (confirmandoSalida_)
    {
        confirmarSalidaRestante_ -= dt;
        if (confirmarSalidaRestante_ <= 0.f) confirmandoSalida_ = false;
    }

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

    const bool peleaVisibleAntes = escenaPelea_.visible();
    escenaPelea_.actualizar(dt);
    if (peleaVisibleAntes && !escenaPelea_.visible())
    {
        vista_.establecerPosicion(centroEscenario_);
        vista_.establecerEscala(3.f);
    }
}

void PantallaJuego::actualizarMetrica()
{
    const Actividad actividad = mascota_->actividad();
    const float     avance    = mascota_->progresoActividad();

    textoMetrica_.setString(
        "Actividad: " + nombreActividad(actividad) + "\n" +
        "Estado:    " + mascota_->estado().nombre() + "\n" +
        "Avance:    " + util::aTexto(avance * 100.f) + "%");

    barraProgreso_ = tema::paralelogramo(origenProgreso_,
                                         { anchoProgreso_ * avance, 10.f }, 4.f);
    barraProgreso_.setFillColor(mascota_->ocupada() ? tema::kAcento : tema::kBarraFondo);
}

void PantallaJuego::dibujar(sf::RenderTarget& objetivo) const
{
    if (escenaPelea_.visible())
    {
        objetivo.draw(escenaPelea_);
        return;
    }

    if (escenario_) objetivo.draw(*escenario_);
    objetivo.draw(vista_);

    if (anuncioRestante_ > 0.f)
        objetivo.draw(anuncio_);

    objetivo.draw(hud_);

    if (admin_.metricaVisible())
    {
        panelMetrica_.dibujar(objetivo);
        tema::dibujarConSombra(objetivo, tituloMetrica_);
        objetivo.draw(textoMetrica_);
        objetivo.draw(fondoProgreso_);
        objetivo.draw(barraProgreso_);
    }

    if (mostrarOcupada_)
    {
        panelOcupada_.dibujar(objetivo);
        tema::dibujarConSombra(objetivo, textoOcupada_);
    }

    panelBotonera_.dibujar(objetivo);
    for (const Boton& boton : botones_)
        objetivo.draw(boton);

    objetivo.draw(pie_);

    objetivo.draw(barrido_);

    objetivo.draw(panelComida_);

    if (admin_.visible())
        objetivo.draw(panelAdmin_);
}

bool PantallaJuego::guardar() const
{
    if (!mascota_) return false;
    return GestorGuardado::guardar(*mascota_, GestorGuardado::rutaPorDefecto());
}

}
