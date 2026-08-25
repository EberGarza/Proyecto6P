#pragma once

#include "TipoEstado.hpp"

#include <optional>
#include <string>

namespace vp {

class Mascota;

class Estado
{
public:
    virtual ~Estado() = default;

    virtual void alEntrar(Mascota& mascota);

    virtual void actualizar(Mascota& mascota, float dt) = 0;

    virtual void alSalir(Mascota& mascota);

    virtual TipoEstado tipo() const = 0;

    virtual std::string nombre() const;

    virtual std::string mensaje() const = 0;

    virtual bool permiteInteraccion() const;

protected:

    static std::optional<TipoEstado> transicionCritica(const Mascota& mascota);

    static constexpr float kUmbralHambre     = 20.f;
    static constexpr float kUmbralCansancio  = 20.f;
    static constexpr float kUmbralEnfermedad = 30.f;
    static constexpr float kUmbralFelicidad  = 80.f;
};

}
