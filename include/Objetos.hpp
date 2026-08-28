#pragma once

#include "Objeto.hpp"

namespace vp {

/// Comida: sube la saciedad. Si es la favorita de la especie, sube mas el animo.
class Alimento : public Objeto
{
public:
    Alimento(std::string nombre, int precio, float nutricion, int usos = 1);

    std::string categoria() const override { return "Comida"; }
    std::string detalle() const override;

    float nutricion() const { return nutricion_; }

protected:
    bool aplicar(Mascota& mascota) override;

private:
    float nutricion_;
};

/// Juguete: sube la felicidad a cambio de energia.
class Juguete : public Objeto
{
public:
    Juguete(std::string nombre, int precio, float diversion, int usos = -1);

    std::string categoria() const override { return "Juguete"; }
    std::string detalle() const override;

    float diversion() const { return diversion_; }

protected:
    bool aplicar(Mascota& mascota) override;

private:
    float diversion_;
};

/// Medicina: sube la salud. Solo tiene sentido si la mascota esta enferma.
class Medicina : public Objeto
{
public:
    Medicina(std::string nombre, int precio, float curacion, int usos = 1);

    std::string categoria() const override { return "Medicina"; }
    std::string detalle() const override;

    float curacion() const { return curacion_; }

protected:
    bool aplicar(Mascota& mascota) override;

private:
    float curacion_;
};

/// Articulo de aseo: sube la higiene.
class ArticuloAseo : public Objeto
{
public:
    ArticuloAseo(std::string nombre, int precio, float limpieza, int usos = -1);

    std::string categoria() const override { return "Aseo"; }
    std::string detalle() const override;

    float limpieza() const { return limpieza_; }

protected:
    bool aplicar(Mascota& mascota) override;

private:
    float limpieza_;
};

} // namespace vp
