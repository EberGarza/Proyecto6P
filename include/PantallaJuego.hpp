#pragma once

#include "Boton.hpp"
#include "Hud.hpp"
#include "PanelAdmin.hpp"
#include "Pantalla.hpp"
#include "VistaMascota.hpp"
#include "AdminMenu.hpp"
#include "Inventario.hpp"
#include "Mascota.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
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

    const sf::Font&              fuente_;
    std::unique_ptr<Mascota>     mascota_;
    Inventario                   inventario_;

    VistaMascota                 vista_;
    Hud                          hud_;
    std::vector<Boton>           botones_;

    AdminMenu                    admin_;
    PanelAdmin                   panelAdmin_;

    sf::RectangleShape           escenario_;
    sf::Text                     pie_;
    sf::Vector2f                 tamanoVentana_;
    sf::Vector2f                 raton_ { 0.f, 0.f };
    float                        fps_ = 0.f;
};

} // namespace vp
