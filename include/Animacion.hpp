#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

namespace vp {

struct Cuadro
{
    sf::IntRect  recorte;
    sf::Vector2f origen;
};

class Animacion
{
public:
    Animacion() = default;

    Animacion(const sf::Texture& textura, std::vector<Cuadro> cuadros,
              float duracionCuadro, bool enBucle = true);

    Animacion(const sf::Texture& textura, int anchoCuadro, int altoCuadro,
              int cantidadCuadros, float duracionCuadro, bool enBucle = true);

    void actualizar(sf::Sprite& sprite, float dt);

    void aplicarCuadroActual(sf::Sprite& sprite) const;

    void reiniciar();

    bool  vacia() const           { return cuadros_.empty(); }
    float duracionCuadro() const  { return duracionCuadro_; }
    bool  terminada() const { return terminada_; }
    int   cuadroActual() const { return cuadroActual_; }
    std::size_t cantidad() const  { return cuadros_.size(); }

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

}
