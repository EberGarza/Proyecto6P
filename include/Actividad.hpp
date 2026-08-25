#pragma once

#include <string>

namespace vp {

enum class Actividad
{
    Ninguna,
    Comiendo,
    Jugando,
    Aseandose,
    Curandose,
    Acariciando,
    Presumiendo,
    Durmiendo
};

std::string nombreActividad(Actividad actividad);

enum class AccionMascota
{
    Alimentar,
    Jugar,
    Asear,
    Medicar,
    Dormir,
    Despertar,
    Acariciar,
    Especial
};

struct Permiso
{
    bool        concedido = false;
    std::string motivo;

    explicit operator bool() const { return concedido; }
};

std::string animacionDeActividad(Actividad actividad);

}
