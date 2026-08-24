#pragma once

#include "TipoEstado.hpp"

#include <array>
#include <cstddef>
#include <deque>
#include <string>

namespace vp {

class Inventario;
class Mascota;

/**
 * @brief Menu de desarrollo del proyecto. Nombre en clave: Admin_Menu.
 *
 * Herramienta interna, no forma parte del juego para el jugador. Sirve para
 * dos cosas:
 *   1. Depurar: forzar cualquier estado, rellenar o vaciar barras, acelerar el
 *      reloj y comprobar que las transiciones funcionan sin esperar minutos.
 *   2. Demostrar: permite recorrer los ocho estados del diagrama en segundos.
 *
 * Esta en el nucleo (sin SFML) a proposito, para que tanto la version grafica
 * como la de consola usen exactamente los mismos comandos. La clase PanelAdmin
 * es solo su cara visible en SFML.
 *
 * Acceso: se desbloquea tecleando la secuencia ADMIN, o con intentarCodigo().
 */
class AdminMenu
{
public:
    /// Secuencia de teclas que desbloquea el menu.
    static constexpr const char* kSecuencia = "ADMIN";

    /// Velocidades de simulacion disponibles (segundos de juego por segundo real).
    static constexpr std::array<float, 4> kEscalas { 1.f, 5.f, 20.f, 60.f };

    AdminMenu() = default;

    // ------------------------------------------------------------- Acceso ---

    bool desbloqueado() const { return desbloqueado_; }
    bool visible() const      { return desbloqueado_ && visible_; }

    /// Alimenta la secuencia secreta con una tecla. true si acaba de desbloquear.
    bool registrarTecla(char tecla);

    /// Desbloqueo directo por codigo (lo usa la version de consola).
    bool intentarCodigo(const std::string& codigo);

    void alternarVisible();
    void cerrar()  { visible_ = false; }
    void bloquear();

    // ---------------------------------------------- Comandos sobre la mascota

    void forzarEstado(Mascota& mascota, TipoEstado tipo);
    void rellenarAtributos(Mascota& mascota);
    void vaciarAtributos(Mascota& mascota);

    /// clave: saciedad | felicidad | energia | higiene | salud
    bool establecerAtributo(Mascota& mascota, const std::string& clave, float valor);

    void matar(Mascota& mascota);
    void revivir(Mascota& mascota);
    void envejecer(Mascota& mascota, float dias);

    // ---------------------------------------------------- Trampas activas ---

    bool inmortalidad() const { return inmortalidad_; }
    void alternarInmortalidad();

    // ------------------------------------------------------------ Metrica ---

    /**
     * @brief Panel de telemetria: que hace la mascota en cada momento.
     *
     * Se enciende desde el Admin_Menu y se queda encendido aunque el menu se
     * cierre, que es justo lo que se quiere: sirve para mirar la partida
     * normal y ver la actividad ("Comiendo", "Durmiendo") y su avance sin
     * tener el panel entero tapando la pantalla.
     */
    bool metricaVisible() const { return metrica_; }
    void alternarMetrica();

    /// Se llama en cada tick: mantiene vivas las trampas que lo necesitan.
    void aplicarPorTick(Mascota& mascota, float dt);

    // ------------------------------------------------- Velocidad del reloj --

    float       escalaTiempo() const { return kEscalas[indiceEscala_]; }
    std::size_t indiceEscala() const { return indiceEscala_; }
    void        siguienteEscala();

    // ---------------------------------------------------------- Economia ----

    void agregarMonedas(Inventario& inventario, int cantidad);

    // ----------------------------------------------------------- Registro ---

    const std::deque<std::string>& registro() const { return registro_; }
    std::string ultimaAccion() const;

private:
    void anotar(const std::string& mensaje);

    bool        desbloqueado_  = false;
    bool        visible_       = false;
    bool        inmortalidad_  = false;
    bool        metrica_       = false;
    std::size_t avanceClave_   = 0;   ///< cuantas letras de kSecuencia van
    std::size_t indiceEscala_  = 0;

    std::deque<std::string> registro_;

    static constexpr std::size_t kMaxRegistro   = 12;
    static constexpr float       kSaludMinima   = 10.f;  ///< piso con inmortalidad
};

} // namespace vp
