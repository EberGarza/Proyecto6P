#pragma once

#include <SFML/Graphics.hpp>

namespace vp {

class Mascota;

/**
 * @brief Interfaz de dibujo de la mascota (patron Strategy).
 *
 * El juego no sabe si la mascota se dibuja con sprites o con figuras: solo
 * conoce esta interfaz. Asi el juego funciona aunque todavia no haya arte, y
 * el dia que existan los sprites basta con cambiar la estrategia.
 */
class RenderMascota : public sf::Drawable
{
public:
    ~RenderMascota() override = default;

    /// Sincroniza el dibujo con el estado de la mascota y avanza la animacion.
    virtual void actualizar(const Mascota& mascota, float dt) = 0;

    virtual void establecerPosicion(sf::Vector2f posicion) = 0;
    virtual void establecerEscala(float escala)            = 0;

    /// Nombre de la estrategia, para mostrarlo en el Admin_Menu.
    virtual const char* nombreRender() const = 0;
};

} // namespace vp
