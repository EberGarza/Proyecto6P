#include "Actividad.hpp"

namespace vp {

std::string nombreActividad(Actividad actividad)
{
    switch (actividad)
    {
        case Actividad::Ninguna:     return "-";
        case Actividad::Comiendo:    return "Comiendo";
        case Actividad::Jugando:     return "Jugando";
        case Actividad::Aseandose:   return "Aseandose";
        case Actividad::Curandose:   return "Curandose";
        case Actividad::Acariciando: return "Recibiendo mimos";
        case Actividad::Presumiendo: return "Presumiendo";
        case Actividad::Durmiendo:   return "Durmiendo";
    }
    return "-";
}

std::string animacionDeActividad(Actividad actividad)
{
    switch (actividad)
    {
        case Actividad::Comiendo:    return "Comer";
        case Actividad::Aseandose:   return "Aseo";
        case Actividad::Curandose:   return "Curar";
        case Actividad::Acariciando: return "Acariciar";
        case Actividad::Presumiendo: return "Especial";

        case Actividad::Jugando:
        case Actividad::Durmiendo:
        case Actividad::Ninguna:
            break;
    }
    return {};
}

}
