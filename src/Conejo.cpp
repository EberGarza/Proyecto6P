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
    //                  saciedad  felicidad  energia  higiene
    establecerTasasBase(-1.4f,    -1.0f,     -0.9f,   -0.9f);

    felicidad().establecer(90.f);   // arranca de buen humor
    energia().establecer(100.f);
}

std::string Conejo::descripcion() const
{
    // El genero gramatical va con el sustantivo, no con el sexo del animal, y
    // no todas las especies forman el femenino igual. Por eso cada especie
    // escribe su propia frase en vez de pegarle un articulo a "conejo".
    const std::string quien = (genero() == Genero::Macho) ? "un conejo" : "una coneja";

    return nombre() + " es " + quien + ". Come sin parar y se ensucia "
           "enseguida, pero se le pasa todo con un rato de juego.";
}

bool Conejo::saltar()
{
    if (!estaViva())                    return false;
    if (!estado().permiteInteraccion()) return false;

    if (energia().porDebajoDe(20.f))
    {
        registrar(nombre() + " intenta saltar y se queda a medio brinco.");
        return false;
    }

    energia().modificar(-14.f);
    felicidad().modificar(20.f);
    higiene().modificar(-6.f);       // levanta polvo
    registrar(nombre() + " sale disparado dando brincos. " + sonido());
    return true;
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

} // namespace vp
