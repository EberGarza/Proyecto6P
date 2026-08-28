#include "Utilidades.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>
#include <utility>

namespace vp {
namespace util {
namespace {

// Generador de numeros aleatorios compartido por todo el programa.
// Se construye una sola vez, la primera vez que alguien lo pide.
std::mt19937& motor()
{
    static std::mt19937 generador(std::random_device{}());
    return generador;
}

} // namespace sin nombre

float limitar(float valor, float minimo, float maximo)
{
    return std::max(minimo, std::min(valor, maximo));
}

int aleatorio(int minimo, int maximo)
{
    if (minimo > maximo) std::swap(minimo, maximo);
    std::uniform_int_distribution<int> distribucion(minimo, maximo);
    return distribucion(motor());
}

float aleatorioReal(float minimo, float maximo)
{
    if (minimo > maximo) std::swap(minimo, maximo);
    std::uniform_real_distribution<float> distribucion(minimo, maximo);
    return distribucion(motor());
}

bool probabilidad(float porcentaje)
{
    return aleatorioReal(0.f, 100.f) < porcentaje;
}

std::string formatearTiempo(float segundos)
{
    const int total   = static_cast<int>(segundos);
    const int dias    = total / 86400;
    const int horas   = (total % 86400) / 3600;
    const int minutos = (total % 3600) / 60;

    char buffer[48];
    std::snprintf(buffer, sizeof(buffer), "%dd %02d:%02d", dias, horas, minutos);
    return std::string(buffer);
}

std::string aTexto(float valor)
{
    return std::to_string(static_cast<int>(std::lround(valor)));
}

} // namespace util
} // namespace vp
