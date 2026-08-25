#pragma once

#include <string>

namespace vp {
namespace util {

float limitar(float valor, float minimo, float maximo);

int aleatorio(int minimo, int maximo);

float aleatorioReal(float minimo, float maximo);

bool probabilidad(float porcentaje);

std::string formatearTiempo(float segundos);

std::string aTexto(float valor);

std::string aMinusculas(std::string texto);

}
}
