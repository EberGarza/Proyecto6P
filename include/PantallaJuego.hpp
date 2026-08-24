#pragma once

#include "Boton.hpp"
#include "Hud.hpp"
#include "Tema.hpp"
#include "PanelAdmin.hpp"
#include "Pantalla.hpp"
#include "VistaMascota.hpp"
#include "AdminMenu.hpp"
#include "Inventario.hpp"
#include "Mascota.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace vp {

/**
 * @brief Pantalla principal: la mascota, sus barras y los botones de accion.
 *
 * Contiene la mascota mientras dura la partida y tambien el
 * Admin_Menu, que se activa tecleando la secuencia secreta.
 */
class PantallaJuego : public Pantalla
{
public:
    PantallaJuego(const sf::Font& fuente,
                  std::unique_ptr<Mascota> mascota,
                  sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

    /// Guarda la partida (lo llama Juego al cerrar la ventana).
    bool guardar() const;

    const Mascota* mascota() const { return mascota_.get(); }

private:
    void crearBotones();
    void construirEscenario();
    void construirBarrido();
    void anunciar(const std::string& texto, sf::Color color);

    const sf::Font&              fuente_;
    std::unique_ptr<Mascota>     mascota_;
    Inventario                   inventario_;

    VistaMascota                 vista_;
    Hud                          hud_;
    std::vector<Boton>           botones_;

    AdminMenu                    admin_;
    PanelAdmin                   panelAdmin_;

    /// Fondo del escenario: la mascota repetida en mosaico, compuesta una vez.
    sf::RenderTexture            lienzoEscenario_;
    std::optional<sf::Sprite>    escenario_;

    /// Rejilla de lineas horizontales que imita el barrido de un monitor CRT.
    sf::VertexArray              barrido_;

    tema::PanelBiselado          panelBotonera_;
    sf::Text                     pie_;

    /// Cartel grande en mitad del escenario cuando la mascota cambia de estado.
    sf::Text                     anuncio_;
    float                        anuncioRestante_ = 0.f;
    TipoEstado                   estadoAnunciado_ = TipoEstado::Normal;

    sf::Vector2f                 tamanoVentana_;
    sf::Vector2f                 centroEscenario_ { 0.f, 0.f };
    sf::Vector2f                 raton_ { 0.f, 0.f };
    float                        fps_ = 0.f;
};

} // namespace vp
