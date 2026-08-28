#include "Objeto.hpp"

#include "Mascota.hpp"

#include <utility>

namespace vp {

Objeto::Objeto(std::string nombre, std::string descripcion, int precio, int usos)
    : nombre_(std::move(nombre))
    , descripcion_(std::move(descripcion))
    , precio_(precio)
    , usosRestantes_(usos)
    , usosInfinitos_(usos < 0)   // un numero negativo significa usos ilimitados
{
}

bool Objeto::usar(Mascota& mascota)
{
    if (agotado() && !usosInfinitos_) return false;

    // Plantilla de metodo: el flujo comun vive aqui, el efecto en la derivada.
    if (!aplicar(mascota)) return false;

    if (!usosInfinitos_) --usosRestantes_;
    return true;
}

} // namespace vp
