#pragma once

#include "TipoEstado.hpp"

#include <array>
#include <cstddef>
#include <deque>
#include <string>

namespace vp {

class Inventario;
class Mascota;

class AdminMenu
{
public:

    static constexpr const char* kSecuencia = "ADMIN";

    static constexpr std::array<float, 4> kEscalas { 1.f, 5.f, 20.f, 60.f };

    AdminMenu() = default;

    bool desbloqueado() const { return desbloqueado_; }
    bool visible() const      { return desbloqueado_ && visible_; }

    bool registrarTecla(char tecla);

    bool intentarCodigo(const std::string& codigo);

    void alternarVisible();
    void cerrar()  { visible_ = false; }
    void bloquear();

    void forzarEstado(Mascota& mascota, TipoEstado tipo);
    void rellenarAtributos(Mascota& mascota);
    void vaciarAtributos(Mascota& mascota);

    bool establecerAtributo(Mascota& mascota, const std::string& clave, float valor);

    void matar(Mascota& mascota);
    void revivir(Mascota& mascota);
    void envejecer(Mascota& mascota, float dias);

    bool inmortalidad() const { return inmortalidad_; }
    void alternarInmortalidad();

    bool metricaVisible() const { return metrica_; }
    void alternarMetrica();

    void aplicarPorTick(Mascota& mascota, float dt);

    float       escalaTiempo() const { return kEscalas[indiceEscala_]; }
    std::size_t indiceEscala() const { return indiceEscala_; }
    void        siguienteEscala();

    void agregarMonedas(Inventario& inventario, int cantidad);

    const std::deque<std::string>& registro() const { return registro_; }
    std::string ultimaAccion() const;

private:
    void anotar(const std::string& mensaje);

    bool        desbloqueado_  = false;
    bool        visible_       = false;
    bool        inmortalidad_  = false;
    bool        metrica_       = false;
    std::size_t avanceClave_   = 0;
    std::size_t indiceEscala_  = 0;

    std::deque<std::string> registro_;

    static constexpr std::size_t kMaxRegistro   = 12;
    static constexpr float       kSaludMinima   = 10.f;
};

}
