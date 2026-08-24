#pragma once

#include "Mascota.hpp"

namespace vp {

/**
 * @brief Castor: tranquilo, trabajador y resistente.
 *
 * Es la especie sufrida de las dos: aguanta mejor el descuido y gasta la
 * energia despacio, pero se aburre si no tiene nada que roer y le cuesta mas
 * recuperar el animo.
 */
class Castor : public Mascota
{
public:
    Castor(std::string nombre, Genero genero);

    std::string especie() const override        { return "Castor"; }
    std::string sonido() const override         { return "Croc croc!"; }
    std::string comidaFavorita() const override { return "corteza de sauce"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    /// Accion exclusiva: roer un tronco. Le sube el animo y le da hambre.
    bool roer();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

} // namespace vp
