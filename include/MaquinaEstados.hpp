#pragma once

#include "Estado.hpp"
#include "TipoEstado.hpp"

#include <memory>
#include <optional>

namespace vp {

class Mascota;

class MaquinaEstados
{
public:
    MaquinaEstados();
    ~MaquinaEstados();

    MaquinaEstados(const MaquinaEstados&)            = delete;
    MaquinaEstados& operator=(const MaquinaEstados&) = delete;

    void iniciar(Mascota& mascota, TipoEstado inicial = TipoEstado::Normal);

    void actualizar(Mascota& mascota, float dt);

    void cambiarA(Mascota& mascota, TipoEstado nuevo);

    const Estado& estadoActual() const;
    TipoEstado    tipoActual() const;
    TipoEstado    tipoAnterior() const;
    float         tiempoEnEstado() const;

    static std::unique_ptr<Estado> crear(TipoEstado tipo);

private:
    void aplicarCambio(Mascota& mascota, TipoEstado nuevo);

    std::unique_ptr<Estado>   actual_;
    TipoEstado                anterior_       = TipoEstado::Normal;
    float                     tiempoEnEstado_ = 0.f;
    bool                      actualizando_   = false;
    std::optional<TipoEstado> pendiente_;
};

}
