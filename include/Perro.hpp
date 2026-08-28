#pragma once

#include "Mascota.hpp"

namespace vp {

/**
 * @brief Perro: energico y sociable.
 *
 * Se aburre rapido (la felicidad baja mas que en las otras especies) y se
 * ensucia con facilidad, pero aguanta bien el hambre y tiene mucha energia.
 */
class Perro : public Mascota
{
public:
    explicit Perro(std::string nombre);

    std::string especie() const override        { return "Perro"; }
    std::string sonido() const override         { return "Guau guau!"; }
    std::string comidaFavorita() const override { return "un hueso"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

} // namespace vp
