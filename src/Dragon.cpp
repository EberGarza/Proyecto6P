#include "Dragon.hpp"

#include <utility>

namespace vp {

Dragon::Dragon(std::string nombre)
    : Mascota(std::move(nombre))
{
    configurarRasgos();
}

void Dragon::configurarRasgos()
{
    //                  saciedad  felicidad  energia  higiene
    establecerTasasBase(-1.8f,    -0.7f,     -0.9f,   -1.4f);

    salud().establecer(100.f);
    saciedad().establecer(70.f);   // siempre llega con hambre
}

std::string Dragon::descripcion() const
{
    return nombre() + " es un dragon. Come el triple que cualquier mascota y "
           "deja todo hecho un desastre, pero es muy resistente.";
}

bool Dragon::escupirFuego()
{
    if (!estaViva())                     return false;
    if (!estado().permiteInteraccion())  return false;
    if (energia().porDebajoDe(25.f))
    {
        registrar(nombre() + " no tiene fuerzas ni para una chispa.");
        return false;
    }

    energia().modificar(-20.f);
    felicidad().modificar(15.f);
    higiene().modificar(-10.f);
    registrar(nombre() + " escupio una llamarada. " + sonido());
    return true;
}

void Dragon::alCambiarEstado(TipoEstado, TipoEstado nuevo)
{
    if (nuevo == TipoEstado::Enferma)
        registrar("Al dragon le sale humo por la nariz. Mala senal.");
}

} // namespace vp
