#pragma once

#include <memory>
#include <string>

namespace vp {

class Mascota;

class GestorGuardado
{
public:

    static bool guardar(const Mascota& mascota, const std::string& ruta);

    static std::unique_ptr<Mascota> cargar(const std::string& ruta);

    static bool existePartida(const std::string& ruta);

    static std::string rutaPorDefecto();

private:
    static constexpr int kVersionFormato = 1;
};

}
