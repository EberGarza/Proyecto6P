#include "MaquinaEstados.hpp"

#include "Estados.hpp"
#include "Mascota.hpp"

namespace vp {

MaquinaEstados::MaquinaEstados()  = default;
MaquinaEstados::~MaquinaEstados() = default;

std::unique_ptr<Estado> MaquinaEstados::crear(TipoEstado tipo)
{
    switch (tipo)
    {
        case TipoEstado::Normal:     return std::make_unique<EstadoNormal>();
        case TipoEstado::Feliz:      return std::make_unique<EstadoFeliz>();
        case TipoEstado::Hambrienta: return std::make_unique<EstadoHambrienta>();
        case TipoEstado::Cansada:    return std::make_unique<EstadoCansada>();
        case TipoEstado::Durmiendo:  return std::make_unique<EstadoDurmiendo>();
        case TipoEstado::Enferma:    return std::make_unique<EstadoEnferma>();
        case TipoEstado::Jugando:    return std::make_unique<EstadoJugando>();
        case TipoEstado::Muerta:     return std::make_unique<EstadoMuerta>();
    }
    return std::make_unique<EstadoNormal>();
}

void MaquinaEstados::iniciar(Mascota& mascota, TipoEstado inicial)
{
    actual_         = crear(inicial);
    anterior_       = inicial;
    tiempoEnEstado_ = 0.f;
    pendiente_.reset();
    actual_->alEntrar(mascota);
}

void MaquinaEstados::actualizar(Mascota& mascota, float dt)
{
    if (!actual_) return;

    actualizando_ = true;
    actual_->actualizar(mascota, dt);
    actualizando_ = false;

    if (pendiente_)
    {
        const TipoEstado nuevo = *pendiente_;
        pendiente_.reset();
        aplicarCambio(mascota, nuevo);
    }

    tiempoEnEstado_ += dt;
}

void MaquinaEstados::cambiarA(Mascota& mascota, TipoEstado nuevo)
{
    if (actualizando_)
    {

        pendiente_ = nuevo;
        return;
    }
    aplicarCambio(mascota, nuevo);
}

void MaquinaEstados::aplicarCambio(Mascota& mascota, TipoEstado nuevo)
{
    if (actual_ && actual_->tipo() == nuevo) return;

    if (actual_)
    {
        anterior_ = actual_->tipo();
        actual_->alSalir(mascota);
    }

    actual_         = crear(nuevo);
    tiempoEnEstado_ = 0.f;
    actual_->alEntrar(mascota);
}

const Estado& MaquinaEstados::estadoActual() const
{
    return *actual_;
}

TipoEstado MaquinaEstados::tipoActual() const
{
    return actual_ ? actual_->tipo() : TipoEstado::Normal;
}

TipoEstado MaquinaEstados::tipoAnterior() const   { return anterior_; }
float      MaquinaEstados::tiempoEnEstado() const { return tiempoEnEstado_; }

}
