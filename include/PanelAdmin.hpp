#pragma once

#include "Boton.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

namespace vp {

class AdminMenu;
class Inventario;
class Mascota;
class VistaMascota;

/**
 * @brief Cara visible del Admin_Menu: superposicion con los controles de prueba.
 *
 * No contiene logica propia. Cada boton delega en un comando de AdminMenu, que
 * vive en el nucleo y no depende de SFML. Esta clase solo se encarga de
 * dibujarlo y de repartir los clics.
 */
class PanelAdmin : public sf::Drawable
{
public:
    PanelAdmin(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    /// Construye los botones. Hay que llamarlo cuando ya existe la mascota.
    void enlazar(AdminMenu& admin, Mascota& mascota,
                 Inventario& inventario, VistaMascota& vista);

    /// Devuelve true si el clic lo consumio el panel.
    bool procesarClic(sf::Vector2f punto);

    void actualizar(sf::Vector2f raton, const AdminMenu& admin,
                    const Mascota& mascota, const VistaMascota& vista,
                    float fps);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    sf::RectangleShape velo_;       ///< oscurece lo que hay detras
    sf::RectangleShape panel_;
    sf::RectangleShape franja_;     ///< banda de color del titulo
    sf::Text           titulo_;
    sf::Text           aviso_;
    sf::Text           diagnostico_;
    sf::Text           registro_;
    sf::Text           seccionEstados_;
    sf::Text           seccionUtiles_;

    const sf::Font&    fuente_;
    std::vector<Boton> botones_;
    sf::Vector2f       origenPanel_;
    bool               enlazado_ = false;

    static constexpr float kAncho = 560.f;
    static constexpr float kAlto  = 508.f;
};

} // namespace vp
