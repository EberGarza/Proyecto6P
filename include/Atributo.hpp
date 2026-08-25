#pragma once

#include <string>

namespace vp {

class Atributo
{
public:
    Atributo(std::string nombre,
             float valorInicial   = 100.f,
             float tasaPorSegundo = 0.f,
             float minimo         = 0.f,
             float maximo         = 100.f);

    void actualizar(float dt);

    void modificar(float cantidad);

    void establecer(float valor);

    const std::string& nombre() const;
    float valor() const;
    float minimo() const;
    float maximo() const;

    float porcentaje() const;

    float tasa() const;
    void  establecerTasa(float tasaPorSegundo);

    bool enMinimo() const;
    bool enMaximo() const;
    bool porDebajoDe(float umbral) const;
    bool porEncimaDe(float umbral) const;

private:
    std::string nombre_;
    float       minimo_;
    float       maximo_;
    float       valor_;
    float       tasaPorSegundo_;
};

}
