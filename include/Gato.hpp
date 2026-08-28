#pragma once

#include "Mascota.hpp"

namespace vp {

/**
 * @brief Gato: independiente y dormilon.
 *
 * Se limpia solo (la higiene casi no baja) y gasta poca energia, pero es
 * exigente con la comida y su animo cae si no le hacen caso.
 */
class Gato : public Mascota
{
public:
    explicit Gato(std::string nombre);

    std::string especie() const override        { return "Gato"; }
    std::string sonido() const override         { return "Miau!"; }
    std::string comidaFavorita() const override { return "pescado"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    /// Los gatos se acicalan solos: recupera algo de higiene con el tiempo.
    void acicalarse();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

} // namespace vp
