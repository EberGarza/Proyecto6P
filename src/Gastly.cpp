#include "Gastly.hpp"

#include <utility>

namespace vp {

Gastly::Gastly(std::string nombre)
    : Mascota(std::move(nombre))
{
    configurarRasgos();
}

void Gastly::configurarRasgos()
{
    //                  saciedad  felicidad  energia  higiene
    establecerTasasBase(-1.0f,    -1.1f,     -1.2f,   -0.3f);

    energia().establecer(70.f);      // es nocturno: de dia anda flojo
    higiene().establecer(100.f);     // es gas, cuesta mucho ensuciarlo
}

std::string Gastly::descripcion() const
{
    return nombre() + " es un Gastly, una mascota hecha de gas. Casi no se "
           "ensucia, pero es nocturno y se aburre si lo ignoras.";
}

bool Gastly::asustar()
{
    if (!estaViva())                    return false;
    if (!estado().permiteInteraccion()) return false;

    if (energia().porDebajoDe(20.f))
    {
        registrar(nombre() + " no tiene energia ni para un susto flojo.");
        return false;
    }

    energia().modificar(-15.f);
    felicidad().modificar(18.f);
    registrar(nombre() + " se materializo de golpe. BUUU! " + sonido());
    return true;
}

void Gastly::alCambiarEstado(TipoEstado, TipoEstado nuevo)
{
    if (nuevo == TipoEstado::Durmiendo)
        registrar(nombre() + " se disolvio en una nube de gas.");
    else if (nuevo == TipoEstado::Feliz)
        registrar(nombre() + " flota dando vueltas de alegria.");
}

} // namespace vp
