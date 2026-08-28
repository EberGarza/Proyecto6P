#include "Atributo.hpp"
#include "Utilidades.hpp"

#include <utility>

namespace vp {

Atributo::Atributo(std::string nombre, float valorInicial, float tasaPorSegundo,
                   float minimo, float maximo)
    : nombre_(std::move(nombre))
    , minimo_(minimo)
    , maximo_(maximo)
    , valor_(util::limitar(valorInicial, minimo, maximo))
    , tasaPorSegundo_(tasaPorSegundo)
{
}

void Atributo::actualizar(float dt)
{
    modificar(tasaPorSegundo_ * dt);
}

void Atributo::modificar(float cantidad)
{
    establecer(valor_ + cantidad);
}

void Atributo::establecer(float valor)
{
    valor_ = util::limitar(valor, minimo_, maximo_);
}

const std::string& Atributo::nombre() const { return nombre_; }

float Atributo::valor() const  { return valor_; }
float Atributo::minimo() const { return minimo_; }
float Atributo::maximo() const { return maximo_; }
float Atributo::tasa() const   { return tasaPorSegundo_; }

float Atributo::porcentaje() const
{
    const float rango = maximo_ - minimo_;
    if (rango <= 0.f) return 0.f;
    return (valor_ - minimo_) / rango;
}

void Atributo::establecerTasa(float tasaPorSegundo)
{
    tasaPorSegundo_ = tasaPorSegundo;
}

bool Atributo::enMinimo() const                { return valor_ <= minimo_; }
bool Atributo::enMaximo() const                { return valor_ >= maximo_; }
bool Atributo::porDebajoDe(float umbral) const { return valor_ < umbral; }
bool Atributo::porEncimaDe(float umbral) const { return valor_ > umbral; }

} // namespace vp
