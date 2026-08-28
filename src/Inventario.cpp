#include "Inventario.hpp"

#include "Mascota.hpp"
#include "Objetos.hpp"

#include <algorithm>
#include <utility>

namespace vp {

Inventario::Inventario() = default;

void Inventario::agregar(std::unique_ptr<Objeto> objeto)
{
    if (objeto) objetos_.push_back(std::move(objeto));
}

bool Inventario::usar(std::size_t indice, Mascota& mascota)
{
    if (indice >= objetos_.size()) return false;

    const bool exito = objetos_[indice]->usar(mascota);
    if (exito) limpiarAgotados();
    return exito;
}

void Inventario::limpiarAgotados()
{
    objetos_.erase(
        std::remove_if(objetos_.begin(), objetos_.end(),
                       [](const std::unique_ptr<Objeto>& o)
                       {
                           return !o->esInfinito() && o->agotado();
                       }),
        objetos_.end());
}

const Objeto* Inventario::objeto(std::size_t indice) const
{
    if (indice >= objetos_.size()) return nullptr;
    return objetos_[indice].get();
}

std::vector<std::size_t> Inventario::indicesPorCategoria(const std::string& categoria) const
{
    std::vector<std::size_t> resultado;
    for (std::size_t i = 0; i < objetos_.size(); ++i)
        if (objetos_[i]->categoria() == categoria)
            resultado.push_back(i);
    return resultado;
}

void Inventario::agregarMonedas(int cantidad)
{
    if (cantidad > 0) monedas_ += cantidad;
}

bool Inventario::gastarMonedas(int cantidad)
{
    if (cantidad <= 0 || monedas_ < cantidad) return false;
    monedas_ -= cantidad;
    return true;
}

Inventario Inventario::inicial()
{
    Inventario inv;

    inv.agregar(std::make_unique<Alimento>("croquetas", 5, 25.f, 5));
    inv.agregar(std::make_unique<Alimento>("pescado",  10, 40.f, 2));
    inv.agregar(std::make_unique<Alimento>("un hueso",  8, 35.f, 2));

    inv.agregar(std::make_unique<Juguete>("pelota", 15, 20.f));
    inv.agregar(std::make_unique<Juguete>("laser",  25, 30.f));

    inv.agregar(std::make_unique<Medicina>("jarabe", 20, 40.f, 2));

    inv.agregar(std::make_unique<ArticuloAseo>("jabon", 12, 50.f));

    return inv;
}

} // namespace vp
