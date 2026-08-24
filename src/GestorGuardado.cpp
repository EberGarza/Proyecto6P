#include "GestorGuardado.hpp"

#include "FabricaMascotas.hpp"
#include "Mascota.hpp"

#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace vp {
namespace {

/// Lee un archivo clave=valor y lo devuelve como diccionario.
std::map<std::string, std::string> leerCampos(std::istream& entrada)
{
    std::map<std::string, std::string> campos;
    std::string linea;

    while (std::getline(entrada, linea))
    {
        // Tolera archivos guardados en Windows (terminados en CR LF).
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty() || linea.front() == '#') continue;

        const std::size_t separador = linea.find('=');
        if (separador == std::string::npos) continue;

        campos[linea.substr(0, separador)] = linea.substr(separador + 1);
    }
    return campos;
}

float aReal(const std::map<std::string, std::string>& campos,
            const std::string& clave, float porDefecto)
{
    auto it = campos.find(clave);
    if (it == campos.end()) return porDefecto;

    try                       { return std::stof(it->second); }
    catch (const std::exception&) { return porDefecto; }
}

int aEntero(const std::map<std::string, std::string>& campos,
            const std::string& clave, int porDefecto)
{
    auto it = campos.find(clave);
    if (it == campos.end()) return porDefecto;

    try                       { return std::stoi(it->second); }
    catch (const std::exception&) { return porDefecto; }
}

std::string aTextoCampo(const std::map<std::string, std::string>& campos,
                        const std::string& clave, const std::string& porDefecto)
{
    auto it = campos.find(clave);
    return it == campos.end() ? porDefecto : it->second;
}

} // namespace sin nombre

bool GestorGuardado::guardar(const Mascota& mascota, const std::string& ruta)
{
    std::ofstream archivo(ruta);
    if (!archivo) return false;

    archivo << "# Partida de VirtualPet\n";
    archivo << "version="   << kVersionFormato               << '\n';
    archivo << "especie="   << mascota.especie()             << '\n';
    archivo << "genero="    << nombreGenero(mascota.genero()) << '\n';
    archivo << "nombre="    << mascota.nombre()              << '\n';
    archivo << "edad="      << mascota.edad()                << '\n';
    archivo << "saciedad="  << mascota.saciedad().valor()    << '\n';
    archivo << "felicidad=" << mascota.felicidad().valor()   << '\n';
    archivo << "energia="   << mascota.energia().valor()     << '\n';
    archivo << "higiene="   << mascota.higiene().valor()     << '\n';
    archivo << "salud="     << mascota.salud().valor()       << '\n';
    archivo << "estado="    << static_cast<int>(mascota.tipoEstado()) << '\n';

    return archivo.good();
}

std::unique_ptr<Mascota> GestorGuardado::cargar(const std::string& ruta)
{
    std::ifstream archivo(ruta);
    if (!archivo) return nullptr;

    const auto campos = leerCampos(archivo);

    const std::string especie = aTextoCampo(campos, "especie", "");
    const std::string nombre  = aTextoCampo(campos, "nombre",  "Sin nombre");
    if (especie.empty()) return nullptr;

    // Las partidas anteriores al genero no traen el campo. Se asume macho en
    // vez de rechazar el archivo: perder una partida por un campo que no
    // existia cuando se guardo seria peor que empezar con el sprite ajeno.
    Genero genero = Genero::Macho;
    generoDesdeNombre(aTextoCampo(campos, "genero", "Macho"), genero);

    auto mascota = FabricaMascotas::crear(especie, genero, nombre);
    if (!mascota) return nullptr;

    mascota->establecerEdad(aReal(campos, "edad", 0.f));
    mascota->saciedad().establecer (aReal(campos, "saciedad",  100.f));
    mascota->felicidad().establecer(aReal(campos, "felicidad",  80.f));
    mascota->energia().establecer  (aReal(campos, "energia",   100.f));
    mascota->higiene().establecer  (aReal(campos, "higiene",   100.f));
    mascota->salud().establecer    (aReal(campos, "salud",     100.f));

    const int estadoGuardado = aEntero(campos, "estado", 0);
    const auto estados = todosLosEstados();
    if (estadoGuardado >= 0 && estadoGuardado < static_cast<int>(estados.size()))
        mascota->cambiarEstado(estados[static_cast<std::size_t>(estadoGuardado)]);

    // Un tick de cero segundos sincroniza banderas derivadas, como estaViva().
    mascota->actualizar(0.f);

    return mascota;
}

bool GestorGuardado::existePartida(const std::string& ruta)
{
    // std::filesystem::exists es la pregunta exacta que queremos hacer. Abrir
    // el archivo con un ifstream y mirar good() responde "se pudo abrir", que
    // no es lo mismo: depende de permisos y de bloqueos de otros procesos.
    std::error_code error;
    return std::filesystem::exists(ruta, error) && !error;
}

std::string GestorGuardado::rutaPorDefecto()
{
    return "partida.txt";
}

} // namespace vp
