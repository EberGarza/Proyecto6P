#pragma once

#include <SFML/Graphics.hpp>

#include <string>

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

    /**
     * @brief Reproduce una vez la animacion de una accion, si existe.
     *
     * No es virtual pura a proposito: una estrategia que no sepa dibujar
     * acciones (el dibujo procedural) no tiene por que implementarla, y quien
     * la llama no deberia preguntar antes. Sin animacion, no pasa nada.
     */
    virtual void reproducirAccion(const std::string& nombre) { (void)nombre; }

    /// Nombre de la estrategia, para mostrarlo en el Admin_Menu.
    virtual const char* nombreRender() const = 0;
};

} // namespace vp
