#pragma once

#include "Boton.hpp"
#include "EscenaPelea.hpp"
#include "Hud.hpp"
#include "Music.hpp"
#include "Tema.hpp"
#include "PanelAdmin.hpp"
#include "PanelComida.hpp"
#include "Pantalla.hpp"
#include "VistaMascota.hpp"
#include "AdminMenu.hpp"
#include "Inventario.hpp"
#include "Mascota.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace vp {

class PantallaJuego : public Pantalla
{
public:
    PantallaJuego(const sf::Font& fuente,
                  std::unique_ptr<Mascota> mascota,
                  sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

    bool guardar() const;

    const Mascota* mascota() const { return mascota_.get(); }

private:
    void crearBotones();
    void construirEscenario();
    void construirBarrido();
    void construirMetrica();
    void actualizarMetrica();
    void anunciar(const std::string& texto, sf::Color color);

    void abrirDespensa();
    void abrirPelea();

    void usarPrimero(const std::string& categoria);

    void intentarAccion(AccionMascota accion);

    void refrescarBotonera();

    const sf::Font&              fuente_;
    std::unique_ptr<Mascota>     mascota_;
    Inventario                   inventario_;

    VistaMascota                 vista_;
    Hud                          hud_;
    std::vector<Boton>           botones_;

    AdminMenu                    admin_;
    PanelAdmin                   panelAdmin_;
    PanelComida                  panelComida_;
    EscenaPelea                  escenaPelea_;

    std::vector<AccionMascota>   accionDeBoton_;

    tema::PanelBiselado          panelOcupada_;
    sf::Text                     textoOcupada_;
    bool                         mostrarOcupada_ = false;

    tema::PanelBiselado          panelMetrica_;
    sf::Text                     tituloMetrica_;
    sf::Text                     textoMetrica_;
    sf::ConvexShape              fondoProgreso_;
    sf::ConvexShape              barraProgreso_;
    sf::Vector2f                 origenProgreso_ { 0.f, 0.f };
    float                        anchoProgreso_  = 0.f;

    sf::RenderTexture            lienzoEscenario_;
    std::optional<sf::Sprite>    escenario_;

    sf::VertexArray              barrido_;

    tema::PanelBiselado          panelBotonera_;
    sf::Text                     pie_;

    sf::Text                     anuncio_;
    float                        anuncioRestante_ = 0.f;
    TipoEstado                   estadoAnunciado_ = TipoEstado::Normal;

    bool                         confirmandoSalida_        = false;
    float                        confirmarSalidaRestante_  = 0.f;

    Music                        musica_;

    sf::Vector2f                 tamanoVentana_;
    sf::Vector2f                 centroEscenario_ { 0.f, 0.f };
    sf::Vector2f                 raton_ { 0.f, 0.f };
    float                        fps_ = 0.f;
};

}
