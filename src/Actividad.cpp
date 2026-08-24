#include "Actividad.hpp"

namespace vp {

std::string nombreActividad(Actividad actividad)
{
    switch (actividad)
    {
        case Actividad::Ninguna:    return "-";
        case Actividad::Comiendo:   return "Comiendo";
        case Actividad::Jugando:    return "Jugando";
        case Actividad::Aseandose:  return "Aseandose";
        case Actividad::Curandose:  return "Curandose";
        case Actividad::Durmiendo:  return "Durmiendo";
    }
    return "-";
}

std::string animacionDeActividad(Actividad actividad)
{
    switch (actividad)
    {
        case Actividad::Comiendo:  return "Comer";
        case Actividad::Aseandose: return "Aseo";

        // Jugando y Durmiendo ya tienen estado propio con su animacion, asi que
        // no hace falta una de accion. Curandose no tiene dibujo en la hoja.
        case Actividad::Jugando:
        case Actividad::Durmiendo:
        case Actividad::Curandose:
        case Actividad::Ninguna:
            break;
    }
    return {};
}

} // namespace vp
