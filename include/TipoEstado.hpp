#pragma once

#include <string>
#include <vector>

namespace vp {

enum class TipoEstado
{
    Normal,
    Feliz,
    Hambrienta,
    Cansada,
    Durmiendo,
    Enferma,
    Jugando,
    Muerta
};

std::string nombreEstado(TipoEstado tipo);

std::vector<TipoEstado> todosLosEstados();

bool estadoDesdeNombre(const std::string& nombre, TipoEstado& destino);

}
