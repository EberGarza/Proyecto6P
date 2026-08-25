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

    establecerTasasBase(-0.10f,   -0.15f,    -0.08f,  -0.09f);

    energia().establecer(100.f);
    felicidad().establecer(70.f);
}

std::string Castor::descripcion() const
{

    const std::string quien = (genero() == Genero::Macho) ? "un castor"
                                                          : "una hembra de castor";

    return nombre() + " es " + quien + ". Aguanta el descuido mejor que "
           "nadie, pero se aburre si no tiene nada que roer.";
}

bool Castor::roer()
{
    const Permiso permiso = puede(AccionMascota::Especial);
    if (!permiso) { registrar(permiso.motivo); return false; }

    energia().modificar(-10.f);
    saciedad().modificar(-8.f);

    return iniciarAccionEspecial(16.f, 1.0f,
                                 nombre() + " parte un tronco de un mordisco. " + sonido());
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

}
