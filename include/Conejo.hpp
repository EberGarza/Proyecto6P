#pragma once

#include "Mascota.hpp"

namespace vp {

/**
 * @brief Conejo: nervioso, alegre y con mucha hambre.
 *
 * Es la especie inquieta de las dos. Se aburre y se ensucia rapido, y come
 * como si no hubiera un manana, pero recupera el animo con nada. Frente al
 * castor, pide mas atencion y perdona mas.
 */
class Conejo : public Mascota
{
public:
    Conejo(std::string nombre, Genero genero);

    std::string especie() const override        { return "Conejo"; }
    std::string sonido() const override         { return "Fsst fsst!"; }
    std::string comidaFavorita() const override { return "una zanahoria"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    /// Accion exclusiva: una carrera de saltos. Sube mucho el animo y cansa.
    bool saltar();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

} // namespace vp
