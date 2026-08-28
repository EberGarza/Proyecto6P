#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

namespace vp {

/**
 * @brief Un cuadro de animacion dentro de una hoja de sprites.
 *
 * El origen es el punto del recorte que se mantiene fijo al dibujar. Sirve
 * para que la mascota no salte cuando los cuadros tienen tamanos distintos o
 * incluyen objetos a un lado (la fruta al comer, la pelota al jugar).
 */
struct Cuadro
{
    sf::IntRect  recorte;
    sf::Vector2f origen;
};

/**
 * @brief Animacion por cuadros sobre una hoja de sprites.
 *
 * Admite dos formas de definir los cuadros:
 *   - una rejilla uniforme (cuadros del mismo tamano, en fila);
 *   - una lista de recortes arbitrarios, que es lo que necesitan las hojas
 *     dibujadas a mano, donde cada pose ocupa un sitio distinto.
 */
class Animacion
{
public:
    Animacion() = default;

    /// Cuadros arbitrarios: cada uno con su recorte y su punto de anclaje.
    Animacion(const sf::Texture& textura, std::vector<Cuadro> cuadros,
              float duracionCuadro, bool enBucle = true);

    /// Rejilla uniforme: n cuadros del mismo tamano, en una fila horizontal.
    Animacion(const sf::Texture& textura, int anchoCuadro, int altoCuadro,
              int cantidadCuadros, float duracionCuadro, bool enBucle = true);

    /// Avanza el tiempo y deja el sprite listo para dibujarse.
    void actualizar(sf::Sprite& sprite, float dt);

    /// Coloca el primer cuadro sin avanzar el reloj.
    void aplicarCuadroActual(sf::Sprite& sprite) const;

    void reiniciar();

    bool  vacia() const           { return cuadros_.empty(); }
    float duracionCuadro() const  { return duracionCuadro_; }
    bool  terminada() const { return terminada_; }
    int   cuadroActual() const { return cuadroActual_; }
    std::size_t cantidad() const  { return cuadros_.size(); }

    /// Acceso a un cuadro concreto. Lo usa la herramienta de revision de hojas.
    const Cuadro& cuadro(std::size_t indice) const { return cuadros_[indice]; }

private:
    const sf::Texture*  textura_         = nullptr;
    std::vector<Cuadro> cuadros_;
    float               duracionCuadro_  = 0.15f;
    float               tiempoAcumulado_ = 0.f;
    int                 cuadroActual_    = 0;
    bool                enBucle_         = true;
    bool                terminada_       = false;
};

} // namespace vp
