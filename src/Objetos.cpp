#include "Objetos.hpp"

#include "Mascota.hpp"
#include "Utilidades.hpp"

#include <utility>

namespace vp {

// -------------------------------------------------------------- Alimento ---

Alimento::Alimento(std::string nombre, int precio, float nutricion, int usos)
    : Objeto(std::move(nombre), "Sube la saciedad", precio, usos)
    , nutricion_(nutricion)
{
}

bool Alimento::aplicar(Mascota& mascota)
{
    if (!mascota.alimentar(nutricion_, nombre())) return false;

    // Extra de felicidad si es justo lo que le gusta a esta especie.
    if (mascota.comidaFavorita().find(nombre()) != std::string::npos ||
        nombre().find(mascota.comidaFavorita()) != std::string::npos)
    {
        mascota.felicidad().modificar(10.f);
        mascota.registrar("Es su comida favorita! " + mascota.sonido());
    }
    return true;
}

std::string Alimento::detalle() const
{
    return "+" + util::aTexto(nutricion_) + " saciedad";
}

// --------------------------------------------------------------- Juguete ---

Juguete::Juguete(std::string nombre, int precio, float diversion, int usos)
    : Objeto(std::move(nombre), "Sube la felicidad", precio, usos)
    , diversion_(diversion)
{
}

bool Juguete::aplicar(Mascota& mascota)
{
    return mascota.jugar(diversion_, nombre());
}

std::string Juguete::detalle() const
{
    return "+" + util::aTexto(diversion_) + " felicidad";
}

// -------------------------------------------------------------- Medicina ---

Medicina::Medicina(std::string nombre, int precio, float curacion, int usos)
    : Objeto(std::move(nombre), "Sube la salud", precio, usos)
    , curacion_(curacion)
{
}

bool Medicina::aplicar(Mascota& mascota)
{
    return mascota.medicar(curacion_, nombre());
}

std::string Medicina::detalle() const
{
    return "+" + util::aTexto(curacion_) + " salud";
}

// ---------------------------------------------------------- ArticuloAseo ---

ArticuloAseo::ArticuloAseo(std::string nombre, int precio, float limpieza, int usos)
    : Objeto(std::move(nombre), "Sube la higiene", precio, usos)
    , limpieza_(limpieza)
{
}

bool ArticuloAseo::aplicar(Mascota& mascota)
{
    return mascota.asear(limpieza_, nombre());
}

std::string ArticuloAseo::detalle() const
{
    return "+" + util::aTexto(limpieza_) + " higiene";
}

} // namespace vp
