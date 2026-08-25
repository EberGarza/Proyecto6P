#pragma once

#include "Mascota.hpp"

namespace vp {

class Conejo : public Mascota
{
public:
    Conejo(std::string nombre, Genero genero);

    std::string especie() const override        { return "Conejo"; }
    std::string sonido() const override         { return "Fsst fsst!"; }
    std::string comidaFavorita() const override { return "una zanahoria"; }

    void configurarRasgos() override;
    std::string descripcion() const override;

    bool saltar();

protected:
    void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo) override;
};

}
