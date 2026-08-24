#pragma once

#include <string>

namespace vp {

class Mascota;

/**
 * @brief Clase base abstracta de todo lo que el jugador puede usar con la mascota.
 *
 * Cada objeto sabe aplicarse a si mismo. La mascota no necesita conocer los
 * tipos concretos: recibe el efecto ya calculado. Asi, agregar una comida o un
 * juguete nuevo no obliga a modificar la clase Mascota.
 */
class Objeto
{
public:
    Objeto(std::string nombre, std::string descripcion, int precio, int usos = 1);
    virtual ~Objeto() = default;

    /// Aplica el efecto del objeto. Devuelve false si no se pudo usar.
    bool usar(Mascota& mascota);

    /// Categoria para agrupar en el inventario ("Comida", "Juguete", ...).
    virtual std::string categoria() const = 0;

    /// Texto corto con el efecto, para mostrar en la tienda o el inventario.
    virtual std::string detalle() const = 0;

    const std::string& nombre() const      { return nombre_; }
    const std::string& descripcion() const { return descripcion_; }
    int  precio() const                    { return precio_; }
    int  usosRestantes() const             { return usosRestantes_; }
    /// true si ya no se puede usar. Un objeto de usos ilimitados NUNCA se
    /// agota: si esto devolviera solo "usosRestantes_ <= 0", cada sitio que
    /// pregunte tendria que acordarse de anadir "&& !esInfinito()", y basta
    /// con que uno se olvide para que el jabon desaparezca del inventario.
    bool agotado() const                   { return !usosInfinitos_ && usosRestantes_ <= 0; }
    bool esInfinito() const                { return usosInfinitos_; }

protected:
    /// Efecto concreto sobre la mascota; lo define cada objeto derivado.
    virtual bool aplicar(Mascota& mascota) = 0;

private:
    std::string nombre_;
    std::string descripcion_;
    int         precio_;
    int         usosRestantes_;
    bool        usosInfinitos_;
};

} // namespace vp
