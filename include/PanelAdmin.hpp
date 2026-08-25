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

class PanelAdmin : public sf::Drawable
{
public:
    PanelAdmin(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void enlazar(AdminMenu& admin, Mascota& mascota,
                 Inventario& inventario, VistaMascota& vista);

    bool procesarClic(sf::Vector2f punto);

    void actualizar(sf::Vector2f raton, const AdminMenu& admin,
                    const Mascota& mascota, const VistaMascota& vista,
                    float fps);

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    sf::RectangleShape velo_;
    sf::RectangleShape panel_;
    sf::RectangleShape franja_;
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

    std::size_t        indiceInmortal_ = 0;
    std::size_t        indiceMetrica_  = 0;

    static constexpr float kAncho = 560.f;
    static constexpr float kAlto  = 546.f;
};

}
