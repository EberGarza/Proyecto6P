#pragma once

#include "Mascota.hpp"

namespace vp {

/**
 * @brief Dragon: la especie dificil, pensada como reto.
 *
 * Come muchisimo y se ensucia rapido, pero es resistente: aguanta mejor la
 * enfermedad. Ademas tiene una accion propia, escupirFuego().
 */
class Dragon : public Mascota
{
public:
    explicit Dragon(std::string nombre);

    std::string especie() const override        { return "Dragon"; }
    std::string sonido() const override         { return "GROAAAR!"; }
    std::string comidaFavorita() const override { return "carne asada"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    /// Accion exclusiva del dragon: sube la felicidad pero gasta energia.
    bool escupirFuego();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

} // namespace vp
