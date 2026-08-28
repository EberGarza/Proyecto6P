#pragma once

#include "Estado.hpp"
#include "TipoEstado.hpp"

#include <memory>
#include <optional>

namespace vp {

class Mascota;

/**
 * @brief Contexto del patron State: guarda el estado activo y aplica los cambios.
 *
 * Detalle importante: un estado puede pedir un cambio DESDE DENTRO de su propio
 * metodo actualizar(). Si el cambio se aplicara de inmediato se destruiria el
 * objeto Estado mientras uno de sus metodos sigue en la pila (puntero colgante).
 * Por eso, mientras se esta actualizando, el cambio queda pendiente y se aplica
 * en cuanto actualizar() termina.
 */
class MaquinaEstados
{
public:
    MaquinaEstados();
    ~MaquinaEstados();

    // No tiene sentido copiar una maquina de estados (posee un unique_ptr).
    MaquinaEstados(const MaquinaEstados&)            = delete;
    MaquinaEstados& operator=(const MaquinaEstados&) = delete;

    /// Coloca el estado inicial sin registrar transicion previa.
    void iniciar(Mascota& mascota, TipoEstado inicial = TipoEstado::Normal);

    /// Ejecuta el tick del estado activo y aplica el cambio pendiente, si lo hay.
    void actualizar(Mascota& mascota, float dt);

    /// Solicita un cambio de estado (inmediato o diferido, segun el contexto).
    void cambiarA(Mascota& mascota, TipoEstado nuevo);

    const Estado& estadoActual() const;
    TipoEstado    tipoActual() const;
    TipoEstado    tipoAnterior() const;
    float         tiempoEnEstado() const;

    /// Fabrica: crea la instancia concreta que corresponde a cada TipoEstado.
    static std::unique_ptr<Estado> crear(TipoEstado tipo);

private:
    void aplicarCambio(Mascota& mascota, TipoEstado nuevo);

    std::unique_ptr<Estado>   actual_;
    TipoEstado                anterior_       = TipoEstado::Normal;
    float                     tiempoEnEstado_ = 0.f;
    bool                      actualizando_   = false;
    std::optional<TipoEstado> pendiente_;
};

} // namespace vp
