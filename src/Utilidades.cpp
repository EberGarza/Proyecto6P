#include "Utilidades.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <random>
#include <utility>

namespace vp {
namespace util {
namespace {

std::mt19937& motor()
{
    static std::mt19937 generador(std::random_device{}());
    return generador;
}

}

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

std::string aMinusculas(std::string texto)
{
    for (char& c : texto)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return texto;
}

}
}
