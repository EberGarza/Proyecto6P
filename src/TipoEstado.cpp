#include "TipoEstado.hpp"

#include <cctype>

namespace vp {

std::string nombreEstado(TipoEstado tipo)
{
    switch (tipo)
    {
        case TipoEstado::Normal:     return "Normal";
        case TipoEstado::Feliz:      return "Feliz";
        case TipoEstado::Hambrienta: return "Hambrienta";
        case TipoEstado::Cansada:    return "Cansada";
        case TipoEstado::Durmiendo:  return "Durmiendo";
        case TipoEstado::Enferma:    return "Enferma";
        case TipoEstado::Jugando:    return "Jugando";
        case TipoEstado::Muerta:     return "Muerta";
    }
    return "Desconocido";
}

std::vector<TipoEstado> todosLosEstados()
{
    return {
        TipoEstado::Normal,
        TipoEstado::Feliz,
        TipoEstado::Hambrienta,
        TipoEstado::Cansada,
        TipoEstado::Durmiendo,
        TipoEstado::Enferma,
        TipoEstado::Jugando,
        TipoEstado::Muerta
    };
}

bool estadoDesdeNombre(const std::string& nombre, TipoEstado& destino)
{
    std::string clave;
    clave.reserve(nombre.size());
    for (char c : nombre)
        clave += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    for (TipoEstado tipo : todosLosEstados())
    {
        std::string candidato = nombreEstado(tipo);
        for (char& c : candidato)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (candidato == clave)
        {
            destino = tipo;
            return true;
        }
    }
    return false;
}

}
