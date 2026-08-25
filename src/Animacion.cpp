#include "Animacion.hpp"

#include <utility>

namespace vp {

Animacion::Animacion(const sf::Texture& textura, std::vector<Cuadro> cuadros,
                     float duracionCuadro, bool enBucle)
    : textura_(&textura)
    , cuadros_(std::move(cuadros))
    , duracionCuadro_(duracionCuadro > 0.f ? duracionCuadro : 0.15f)
    , enBucle_(enBucle)
{
}

Animacion::Animacion(const sf::Texture& textura, int anchoCuadro, int altoCuadro,
                     int cantidadCuadros, float duracionCuadro, bool enBucle)
    : textura_(&textura)
    , duracionCuadro_(duracionCuadro > 0.f ? duracionCuadro : 0.15f)
    , enBucle_(enBucle)
{
    cuadros_.reserve(static_cast<std::size_t>(cantidadCuadros));

    for (int i = 0; i < cantidadCuadros; ++i)
    {
        Cuadro cuadro;
        cuadro.recorte = sf::IntRect({ i * anchoCuadro, 0 }, { anchoCuadro, altoCuadro });
        cuadro.origen  = { anchoCuadro * 0.5f, altoCuadro * 0.5f };
        cuadros_.push_back(cuadro);
    }
}

void Animacion::actualizar(sf::Sprite& sprite, float dt)
{
    if (cuadros_.empty()) return;

    if (!terminada_)
    {
        tiempoAcumulado_ += dt;

        while (tiempoAcumulado_ >= duracionCuadro_)
        {
            tiempoAcumulado_ -= duracionCuadro_;
            ++cuadroActual_;

            if (cuadroActual_ >= static_cast<int>(cuadros_.size()))
            {
                if (enBucle_)
                {
                    cuadroActual_ = 0;
                }
                else
                {
                    cuadroActual_ = static_cast<int>(cuadros_.size()) - 1;
                    terminada_    = true;
                    break;
                }
            }
        }
    }

    aplicarCuadroActual(sprite);
}

void Animacion::aplicarCuadroActual(sf::Sprite& sprite) const
{
    if (cuadros_.empty() || !textura_) return;

    const Cuadro& cuadro = cuadros_[static_cast<std::size_t>(cuadroActual_)];

    sprite.setTexture(*textura_);
    sprite.setTextureRect(cuadro.recorte);
    sprite.setOrigin(cuadro.origen);
}

void Animacion::reiniciar()
{
    cuadroActual_    = 0;
    tiempoAcumulado_ = 0.f;
    terminada_       = false;
}

}
