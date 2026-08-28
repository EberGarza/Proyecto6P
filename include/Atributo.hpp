#pragma once

#include <string>

namespace vp {

/**
 * @brief Atributo numerico acotado de la mascota.
 *
 * Representa una barra del juego (saciedad, felicidad, energia, higiene, salud).
 * El valor siempre queda dentro de [minimo, maximo] y puede cambiar solo con el
 * paso del tiempo mediante una tasa por segundo.
 *
 * Convencion del proyecto: en TODOS los atributos, mas alto es mejor.
 *   - saciedad  0 = famelica,  100 = llena
 *   - felicidad 0 = deprimida, 100 = feliz
 *   - energia   0 = agotada,   100 = descansada
 *   - higiene   0 = sucia,     100 = limpia
 *   - salud     0 = muerta,    100 = sana
 */
class Atributo
{
public:
    Atributo(std::string nombre,
             float valorInicial   = 100.f,
             float tasaPorSegundo = 0.f,
             float minimo         = 0.f,
             float maximo         = 100.f);

    /// Aplica el cambio automatico correspondiente a dt segundos.
    void actualizar(float dt);

    /// Suma una cantidad al valor actual (puede ser negativa).
    void modificar(float cantidad);

    /// Fija el valor directamente (se recorta al rango valido).
    void establecer(float valor);

    const std::string& nombre() const;
    float valor() const;
    float minimo() const;
    float maximo() const;

    /// Valor normalizado entre 0.0 y 1.0 (util para dibujar barras).
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
    float       tasaPorSegundo_;   ///< Cambio automatico por segundo (negativo = baja).
};

} // namespace vp
