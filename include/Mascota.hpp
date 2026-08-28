#pragma once

#include "Atributo.hpp"
#include "MaquinaEstados.hpp"
#include "TipoEstado.hpp"

#include <cstddef>
#include <deque>
#include <string>

namespace vp {

class Estado;

/**
 * @brief Clase base abstracta de toda mascota virtual.
 *
 * Concentra lo que comparten todas las especies: los cinco atributos, la edad,
 * la maquina de estados y las acciones del jugador. Cada especie concreta
 * (Perro, Gato, Dragon) hereda de aqui y define su comportamiento propio.
 *
 * Nota de diseno: configurarRasgos() es virtual pura, pero NO se puede llamar
 * desde el constructor de Mascota, porque durante la construccion de la clase
 * base todavia no existe la parte derivada del objeto y el despacho virtual
 * apuntaria a Mascota. Por eso cada constructor derivado la llama en su cuerpo.
 */
class Mascota
{
public:
    explicit Mascota(std::string nombre);
    virtual ~Mascota() = default;

    // Una mascota es una entidad unica: no se copia.
    Mascota(const Mascota&)            = delete;
    Mascota& operator=(const Mascota&) = delete;

    // ------------------------------------------------------- Ciclo de vida --

    /// Avanza la simulacion dt segundos: atributos, efectos y estado.
    void actualizar(float dt);

    // --------------------------------------------- Acciones del jugador -----
    // Todas devuelven true si la accion se pudo realizar.

    bool alimentar(float cantidad);
    bool jugar(float intensidad);
    bool asear(float cantidad);
    bool medicar(float cantidad);
    bool dormir();
    bool despertar();
    bool acariciar();

    /// Devuelve la vida a una mascota muerta. Lo usa el menu de desarrollo y
    /// queda reservado para un futuro objeto de resurreccion.
    void revivir(float saludInicial = 50.f);

    // ------------------------------------------------------------ Consultas -

    const std::string& nombre() const            { return nombre_; }
    void establecerNombre(const std::string& n)  { nombre_ = n; }

    float edad() const                           { return edadSegundos_; }
    void  establecerEdad(float segundos)         { edadSegundos_ = segundos; }
    int   edadEnDias() const;

    bool estaViva() const                        { return viva_; }

    Atributo&       saciedad()        { return saciedad_; }
    const Atributo& saciedad() const  { return saciedad_; }
    Atributo&       felicidad()       { return felicidad_; }
    const Atributo& felicidad() const { return felicidad_; }
    Atributo&       energia()         { return energia_; }
    const Atributo& energia() const   { return energia_; }
    Atributo&       higiene()         { return higiene_; }
    const Atributo& higiene() const   { return higiene_; }
    Atributo&       salud()           { return salud_; }
    const Atributo& salud() const     { return salud_; }

    bool tieneHambre() const;
    bool estaCansada() const;
    bool estaSucia() const;
    bool estaEnferma() const;

    // -------------------------------------------------------- Estados -------

    const Estado&   estado() const;
    TipoEstado      tipoEstado() const;
    MaquinaEstados& maquina()             { return maquina_; }
    void            cambiarEstado(TipoEstado nuevo);

    // -------------------------------------------------------- Bitacora ------

    /// Agrega una linea al historial de eventos que muestra la interfaz.
    void registrar(const std::string& mensaje);
    const std::deque<std::string>& bitacora() const { return bitacora_; }
    std::string ultimoEvento() const;

    // ------------------------------------------------------- Tasas base -----

    /// Vuelve a poner las tasas propias de la especie (tras dormir, curarse...).
    void restaurarTasasBase();

    float tasaBaseSaciedad() const  { return tasaBaseSaciedad_; }
    float tasaBaseFelicidad() const { return tasaBaseFelicidad_; }
    float tasaBaseEnergia() const   { return tasaBaseEnergia_; }
    float tasaBaseHigiene() const   { return tasaBaseHigiene_; }

    // ----------------------------------------------------- Polimorfismo -----

    virtual std::string especie() const        = 0;
    virtual std::string sonido() const         = 0;
    virtual std::string comidaFavorita() const = 0;

    /// Ajusta las tasas y valores iniciales propios de la especie.
    virtual void configurarRasgos() = 0;

    /// Descripcion para la ficha de la mascota; las especies pueden ampliarla.
    virtual std::string descripcion() const;

protected:
    /// Guarda y aplica las tasas de decaimiento propias de la especie.
    void establecerTasasBase(float saciedad, float felicidad,
                             float energia, float higiene);

    /// Gancho opcional: se llama cada vez que cambia el estado.
    virtual void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo);

private:
    /// Efectos cruzados entre atributos (hambre y suciedad danan la salud).
    void aplicarEfectosSecundarios(float dt);

    std::string nombre_;
    float       edadSegundos_ = 0.f;
    bool        viva_         = true;

    Atributo saciedad_;
    Atributo felicidad_;
    Atributo energia_;
    Atributo higiene_;
    Atributo salud_;

    float tasaBaseSaciedad_  = 0.f;
    float tasaBaseFelicidad_ = 0.f;
    float tasaBaseEnergia_   = 0.f;
    float tasaBaseHigiene_   = 0.f;

    MaquinaEstados          maquina_;
    std::deque<std::string> bitacora_;

    static constexpr std::size_t kMaxBitacora = 20;
};

} // namespace vp
