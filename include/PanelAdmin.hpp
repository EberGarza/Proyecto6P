#pragma once

#include "Boton.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
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

    /// Posicion en botones_ de los dos interruptores, para poder marcarlos
    /// mientras esten encendidos. Se guarda el indice y no la etiqueta:
    /// renombrar un boton no deberia romper esto en silencio.
    std::size_t        indiceInmortal_ = 0;
    std::size_t        indiceMetrica_  = 0;

    static constexpr float kAncho = 560.f;
    static constexpr float kAlto  = 546.f;
};

} // namespace vp
