#pragma once

#include "Mascota.hpp"

namespace vp {

class Castor : public Mascota
{
public:
    Castor(std::string nombre, Genero genero);

    std::string especie() const override        { return "Castor"; }
    std::string sonido() const override         { return "Croc croc!"; }
    std::string comidaFavorita() const override { return "corteza de sauce"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    bool roer();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

}
