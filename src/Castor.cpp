#include "Castor.hpp"

#include <utility>

namespace vp {

Castor::Castor(std::string nombre, Genero genero)
    : Mascota(std::move(nombre), genero)
{
    configurarRasgos();
}

void Castor::configurarRasgos()
{
    //                  saciedad  felicidad  energia  higiene
    establecerTasasBase(-0.8f,    -1.2f,     -0.6f,   -0.7f);

    energia().establecer(100.f);
    felicidad().establecer(70.f);   // es serio, cuesta animarlo
}

std::string Castor::descripcion() const
{
    // "Castor" no tiene femenino asentado, asi que la hembra se nombra con un
    // rodeo en vez de inventar una palabra.
    const std::string quien = (genero() == Genero::Macho) ? "un castor"
                                                          : "una hembra de castor";

    return nombre() + " es " + quien + ". Aguanta el descuido mejor que "
           "nadie, pero se aburre si no tiene nada que roer.";
}

bool Castor::roer()
{
    if (!estaViva())                    return false;
    if (!estado().permiteInteraccion()) return false;

    if (energia().porDebajoDe(15.f))
    {
        registrar(nombre() + " mira el tronco, bosteza y lo deja para luego.");
        return false;
    }

    energia().modificar(-10.f);
    felicidad().modificar(16.f);
    saciedad().modificar(-8.f);      // roer abre el apetito
    registrar(nombre() + " parte un tronco de un mordisco. " + sonido());
    return true;
}

void Castor::alCambiarEstado(TipoEstado, TipoEstado nuevo)
{
    if (nuevo == TipoEstado::Durmiendo)
        registrar(nombre() + " se acomoda en la madriguera.");
    else if (nuevo == TipoEstado::Feliz)
        registrar(nombre() + " golpea el suelo con la cola, contento.");
    else if (nuevo == TipoEstado::Cansada)
        registrar(nombre() + " arrastra la cola de puro cansancio.");
}

} // namespace vp
