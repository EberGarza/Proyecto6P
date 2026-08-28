#pragma once

#include "TipoEstado.hpp"

#include <optional>
#include <string>

namespace vp {

class Mascota;   // declaracion adelantada: evita una inclusion circular

/**
 * @brief Clase base abstracta del patron State.
 *
 * Cada estado concreto encapsula:
 *   - que ocurre al entrar (alEntrar),
 *   - que ocurre en cada tick mientras se esta dentro (actualizar),
 *   - que ocurre al salir (alSalir),
 *   - a que otros estados puede transitar.
 *
 * Un estado NUNCA se destruye a si mismo: pide el cambio con
 * Mascota::cambiarEstado y la MaquinaEstados lo aplica cuando es seguro.
 */
class Estado
{
public:
    virtual ~Estado() = default;

    /// Se ejecuta una vez al entrar al estado.
    virtual void alEntrar(Mascota& mascota);

    /// Se ejecuta en cada tick del juego.
    virtual void actualizar(Mascota& mascota, float dt) = 0;

    /// Se ejecuta una vez al salir del estado.
    virtual void alSalir(Mascota& mascota);

    /// Identificador del estado concreto.
    virtual TipoEstado tipo() const = 0;

    /// Nombre legible; por defecto usa nombreEstado(tipo()).
    virtual std::string nombre() const;

    /// Frase que la interfaz muestra mientras la mascota esta en este estado.
    virtual std::string mensaje() const = 0;

    /// false si el jugador no puede interactuar (dormida, muerta).
    virtual bool permiteInteraccion() const;

protected:
    /**
     * @brief Reglas de transicion compartidas, en orden de prioridad.
     *
     * muerte > enfermedad > hambre > cansancio.
     * Devuelve std::nullopt si no hay ninguna necesidad critica.
     */
    static std::optional<TipoEstado> transicionCritica(const Mascota& mascota);

    /// Umbrales usados por todo el sistema de estados.
    static constexpr float kUmbralHambre     = 20.f;
    static constexpr float kUmbralCansancio  = 20.f;
    static constexpr float kUmbralEnfermedad = 30.f;
    static constexpr float kUmbralFelicidad  = 80.f;
};

} // namespace vp
