#pragma once

#include <string>

namespace vp {

class Mascota;

class Objeto
{
public:
    Objeto(std::string nombre, std::string descripcion, int precio, int usos = 1);
    virtual ~Objeto() = default;

    bool usar(Mascota& mascota);

    virtual std::string categoria() const = 0;

    virtual std::string detalle() const = 0;

    const std::string& nombre() const      { return nombre_; }
    const std::string& descripcion() const { return descripcion_; }
    int  precio() const                    { return precio_; }
    int  usosRestantes() const             { return usosRestantes_; }

    bool agotado() const                   { return !usosInfinitos_ && usosRestantes_ <= 0; }
    bool esInfinito() const                { return usosInfinitos_; }

protected:

    virtual bool aplicar(Mascota& mascota) = 0;

private:
    std::string nombre_;
    std::string descripcion_;
    int         precio_;
    int         usosRestantes_;
    bool        usosInfinitos_;
};

}
