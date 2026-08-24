#pragma once

#include <string>

namespace vp {

/**
 * @brief Lo que la mascota esta haciendo ahora mismo.
 *
 * No es lo mismo que su estado. El estado dice **como esta** (hambrienta,
 * enferma, feliz); la actividad dice **que hace** durante unos segundos. Una
 * mascota puede estar hambrienta y comiendo a la vez: lo primero es la
 * situacion, lo segundo es el remedio en marcha.
 *
 * Separarlos permite que las acciones no sean instantaneas: al dar de comer no
 * se suma la saciedad de golpe, sino que la mascota se pone a comer y la barra
 * sube poco a poco mientras la animacion se repite. Cuando termina, la
 * actividad se apaga sola.
 */
enum class Actividad
{
    Ninguna,
    Comiendo,
    Jugando,
    Aseandose,
    Curandose,
    Durmiendo
};

/// Nombre para mostrar: "Comiendo", "Durmiendo"... "-" si no hace nada.
std::string nombreActividad(Actividad actividad);

/// Nombre de la animacion que le corresponde en la hoja de sprites, o cadena
/// vacia si esa actividad no tiene dibujo propio y se queda con el del estado.
std::string animacionDeActividad(Actividad actividad);

} // namespace vp
