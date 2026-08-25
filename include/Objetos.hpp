#pragma once

#include "Objeto.hpp"

namespace vp {

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

}
