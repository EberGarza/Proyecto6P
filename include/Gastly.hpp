#pragma once

#include "Mascota.hpp"

namespace vp {

/**
 * @brief Gastly: mascota fantasma, hecha de gas.
 *
 * Es la especie que tiene hoja de sprites propia en assets/images/.
 * Como es un gas, casi no se ensucia; a cambio es nocturna, gasta energia
 * rapido de dia y su animo depende mucho de que le hagan caso.
 */
class Gastly : public Mascota
{
public:
    explicit Gastly(std::string nombre);

    std::string especie() const override        { return "Gastly"; }
    std::string sonido() const override         { return "Gassss..."; }
    std::string comidaFavorita() const override { return "una fruta"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    /// Accion exclusiva: dar un susto. Le sube mucho el animo, pero cansa.
    bool asustar();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

} // namespace vp
