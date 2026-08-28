#include "Gato.hpp"

#include <utility>

namespace vp {

Gato::Gato(std::string nombre)
    : Mascota(std::move(nombre))
{
    configurarRasgos();
}

void Gato::configurarRasgos()
{
    //                  saciedad  felicidad  energia  higiene
    establecerTasasBase(-1.1f,    -0.9f,     -0.4f,   -0.2f);

    energia().establecer(80.f);
    higiene().establecer(100.f);
}

std::string Gato::descripcion() const
{
    return nombre() + " es un gato independiente. Duerme casi todo el dia y "
           "se limpia solo, pero es muy exigente con la comida.";
}

void Gato::acicalarse()
{
    higiene().modificar(10.f);
    registrar(nombre() + " se acicalo solo.");
}

void Gato::alCambiarEstado(TipoEstado, TipoEstado nuevo)
{
    if (nuevo == TipoEstado::Durmiendo)
        registrar(nombre() + " se enrosco como una dona.");
}

} // namespace vp
