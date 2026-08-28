#pragma once

#include <string>

namespace vp {
namespace util {

/// Recorta @p valor para que quede dentro del rango [minimo, maximo].
float limitar(float valor, float minimo, float maximo);

/// Entero aleatorio dentro del rango cerrado [minimo, maximo].
int aleatorio(int minimo, int maximo);

/// Numero real aleatorio dentro del rango [minimo, maximo].
float aleatorioReal(float minimo, float maximo);

/// Devuelve true con una probabilidad de @p porcentaje (0 a 100).
bool probabilidad(float porcentaje);

/// Convierte segundos de juego a un texto legible, por ejemplo "2d 03:15".
std::string formatearTiempo(float segundos);

/// Convierte un numero real a texto sin decimales (para mensajes de la UI).
std::string aTexto(float valor);

} // namespace util
} // namespace vp
