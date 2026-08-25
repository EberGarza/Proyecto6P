#pragma once

#include "Objeto.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace vp {

class Mascota;

class Inventario
{
public:
    Inventario();

    Inventario(const Inventario&)            = delete;
    Inventario& operator=(const Inventario&) = delete;
    Inventario(Inventario&&)                 = default;
    Inventario& operator=(Inventario&&)      = default;

    void agregar(std::unique_ptr<Objeto> objeto);

    bool usar(std::size_t indice, Mascota& mascota);

    void limpiarAgotados();

    std::size_t   cantidad() const { return objetos_.size(); }
    bool          vacio() const    { return objetos_.empty(); }
    const Objeto* objeto(std::size_t indice) const;

    std::vector<std::size_t> indicesPorCategoria(const std::string& categoria) const;

    int  monedas() const { return monedas_; }
    void agregarMonedas(int cantidad);
    bool gastarMonedas(int cantidad);

    static Inventario inicial();

private:
    std::vector<std::unique_ptr<Objeto>> objetos_;
    int monedas_ = 50;
};

}
