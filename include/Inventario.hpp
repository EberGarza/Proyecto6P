#pragma once

#include "Objeto.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace vp {

class Mascota;

/**
 * @brief Mochila del jugador: guarda los objetos y las monedas.
 *
 * Es dueno de los objetos (unique_ptr), por eso no se puede copiar.
 */
class Inventario
{
public:
    Inventario();

    Inventario(const Inventario&)            = delete;
    Inventario& operator=(const Inventario&) = delete;
    Inventario(Inventario&&)                 = default;
    Inventario& operator=(Inventario&&)      = default;

    /// Toma posesion del objeto.
    void agregar(std::unique_ptr<Objeto> objeto);

    /// Usa el objeto en la posicion indicada. Devuelve false si no se pudo.
    bool usar(std::size_t indice, Mascota& mascota);

    /// Elimina los objetos consumibles que ya no tienen usos.
    void limpiarAgotados();

    std::size_t   cantidad() const { return objetos_.size(); }
    bool          vacio() const    { return objetos_.empty(); }
    const Objeto* objeto(std::size_t indice) const;

    /// Indices de los objetos de una categoria ("Comida", "Juguete", ...).
    std::vector<std::size_t> indicesPorCategoria(const std::string& categoria) const;

    int  monedas() const { return monedas_; }
    void agregarMonedas(int cantidad);
    bool gastarMonedas(int cantidad);

    /// Inventario con el que arranca una partida nueva.
    static Inventario inicial();

private:
    std::vector<std::unique_ptr<Objeto>> objetos_;
    int monedas_ = 50;
};

} // namespace vp
