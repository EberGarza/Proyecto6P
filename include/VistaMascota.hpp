#pragma once

#include "RenderMascota.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <string>

namespace vp {

class Mascota;

/**
 * @brief Fachada de dibujo de la mascota: elige y usa la estrategia adecuada.
 *
 * Al preparar una especie intenta cargar sus sprites; si no hay arte en
 * assets/images/, cae en el dibujo procedural. El resto del juego habla solo
 * con esta clase y no se entera de cual de las dos se esta usando.
 */
class VistaMascota : public sf::Drawable
{
public:
    explicit VistaMascota(const sf::Font& fuente);

    /// Prepara el dibujo para una especie. Devuelve true si uso sprites.
    bool prepararEspecie(const std::string& especie);

    void actualizar(const Mascota& mascota, float dt);
    void establecerPosicion(sf::Vector2f posicion);
    void establecerEscala(float escala);

    /// Cambia de estrategia a mano (lo usa el Admin_Menu).
    void forzarProcedural();

    bool        usandoSprites() const { return usandoSprites_; }
    const char* nombreRender() const;

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;

    const sf::Font&                fuente_;
    std::unique_ptr<RenderMascota> render_;
    sf::Vector2f                   posicion_ { 0.f, 0.f };
    float                          escala_   = 1.f;
    bool                           usandoSprites_ = false;
};

} // namespace vp
