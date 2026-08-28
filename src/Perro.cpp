#include "Perro.hpp"

#include <utility>

namespace vp {

Perro::Perro(std::string nombre)
    : Mascota(std::move(nombre))
{
    // Se llama aqui y no en Mascota porque el despacho virtual no funciona
    // durante la construccion de la clase base.
    configurarRasgos();
}

void Perro::configurarRasgos()
{
    //                  saciedad  felicidad  energia  higiene
    establecerTasasBase(-0.9f,    -1.2f,     -0.7f,   -1.0f);

    energia().establecer(100.f);
    felicidad().establecer(90.f);
}

std::string Perro::descripcion() const
{
    return nombre() + " es un perro inquieto. Si lo dejas solo mucho rato, "
           "se aburre y se pone triste.";
}

void Perro::alCambiarEstado(TipoEstado, TipoEstado nuevo)
{
    if (nuevo == TipoEstado::Feliz)
        registrar(nombre() + " mueve la cola sin parar.");
}

} // namespace vp
