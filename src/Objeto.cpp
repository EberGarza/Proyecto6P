#include "Objeto.hpp"

#include "Mascota.hpp"

#include <utility>

namespace vp {

Objeto::Objeto(std::string nombre, std::string descripcion, int precio, int usos)
    : nombre_(std::move(nombre))
    , descripcion_(std::move(descripcion))
    , precio_(precio)
    , usosRestantes_(usos)
    , usosInfinitos_(usos < 0)
{
}

bool Objeto::usar(Mascota& mascota)
{
    if (agotado()) return false;

    if (!aplicar(mascota)) return false;

    if (!usosInfinitos_) --usosRestantes_;
    return true;
}

}
