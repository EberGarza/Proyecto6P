#include "Conejo.hpp"

#include <utility>

namespace vp {

Conejo::Conejo(std::string nombre, Genero genero)
    : Mascota(std::move(nombre), genero)
{
    configurarRasgos();
}

void Conejo::configurarRasgos()
{

    establecerTasasBase(-0.18f,   -0.13f,    -0.11f,  -0.11f);

    felicidad().establecer(90.f);
    energia().establecer(100.f);
}

std::string Conejo::descripcion() const
{

    const std::string quien = (genero() == Genero::Macho) ? "un conejo" : "una coneja";

    return nombre() + " es " + quien + ". Come sin parar y se ensucia "
           "enseguida, pero se le pasa todo con un rato de juego.";
}

bool Conejo::saltar()
{
    const Permiso permiso = puede(AccionMascota::Especial);
    if (!permiso) { registrar(permiso.motivo); return false; }

    energia().modificar(-14.f);
    higiene().modificar(-6.f);

    return iniciarAccionEspecial(20.f, 0.9f,
                                 nombre() + " sale disparado dando brincos. " + sonido());
}

void Conejo::alCambiarEstado(TipoEstado, TipoEstado nuevo)
{
    if (nuevo == TipoEstado::Durmiendo)
        registrar(nombre() + " se hace una bola y cierra los ojos.");
    else if (nuevo == TipoEstado::Feliz)
        registrar(nombre() + " da saltitos de alegria.");
    else if (nuevo == TipoEstado::Hambrienta)
        registrar(nombre() + " mordisquea el aire buscando comida.");
}

}
