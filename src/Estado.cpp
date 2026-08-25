#include "Estado.hpp"

#include "Mascota.hpp"

namespace vp {

void Estado::alEntrar(Mascota&) {}
void Estado::alSalir(Mascota&)  {}

std::string Estado::nombre() const
{
    return nombreEstado(tipo());
}

bool Estado::permiteInteraccion() const
{
    return true;
}

std::optional<TipoEstado> Estado::transicionCritica(const Mascota& mascota)
{
    if (!mascota.estaViva())                                return TipoEstado::Muerta;
    if (mascota.salud().porDebajoDe(kUmbralEnfermedad))     return TipoEstado::Enferma;
    if (mascota.saciedad().porDebajoDe(kUmbralHambre))      return TipoEstado::Hambrienta;
    if (mascota.energia().porDebajoDe(kUmbralCansancio))    return TipoEstado::Cansada;
    return std::nullopt;
}

}
