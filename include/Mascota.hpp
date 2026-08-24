#pragma once

#include "Actividad.hpp"
#include "Atributo.hpp"
#include "Genero.hpp"
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
 * (Conejo, Castor) hereda de aqui y define su comportamiento propio.
 *
 * El genero vive aqui y no en las especies porque no es un rasgo de la
 * especie: cualquier mascota puede ser macho o hembra, y lo unico que cambia
 * es la hoja de sprites que se carga y como se habla de ella.
 *
 * Nota de diseno: configurarRasgos() es virtual pura, pero NO se puede llamar
 * desde el constructor de Mascota, porque durante la construccion de la clase
 * base todavia no existe la parte derivada del objeto y el despacho virtual
 * apuntaria a Mascota. Por eso cada constructor derivado la llama en su cuerpo.
 */
class Mascota
{
public:
    Mascota(std::string nombre, Genero genero);
    virtual ~Mascota() = default;

    // Una mascota es una entidad unica: no se copia.
    Mascota(const Mascota&)            = delete;
    Mascota& operator=(const Mascota&) = delete;

    // ------------------------------------------------------- Ciclo de vida --

    /// Avanza la simulacion dt segundos: atributos, efectos y estado.
    void actualizar(float dt);

    // --------------------------------------------- Acciones del jugador -----
    // Todas devuelven true si la accion se pudo empezar.
    //
    // Alimentar, jugar, asear y medicar NO son instantaneas: ponen a la mascota
    // a hacer algo durante unos segundos y la barra sube poco a poco. El
    // segundo parametro es que se le esta dando, y solo sirve para la bitacora.

    bool alimentar(float cantidad, const std::string& queCome = "su comida");
    bool jugar(float intensidad, const std::string& conQue = "");
    bool asear(float cantidad, const std::string& conQue = "");
    bool medicar(float cantidad, const std::string& conQue = "su medicina");
    bool dormir();
    bool despertar();
    bool acariciar();

    // ------------------------------------------------------- Actividad ------

    /// Que esta haciendo ahora mismo. Es independiente de su estado.
    Actividad actividad() const;

    /// true mientras una actividad este en marcha y no acepte otra encima.
    bool ocupada() const { return actividadEnCurso_ != Actividad::Ninguna; }

    /// De 0 a 1: cuanto lleva hecho de la actividad actual.
    float progresoActividad() const;

    /// Corta la actividad en curso sin completarla (al dormirse, al morir...).
    void cancelarActividad();

    /// Devuelve la vida a una mascota muerta. Lo usa el menu de desarrollo y
    /// queda reservado para un futuro objeto de resurreccion.
    void revivir(float saludInicial = 50.f);

    // ------------------------------------------------------------ Consultas -

    const std::string& nombre() const            { return nombre_; }
    void establecerNombre(const std::string& n)  { nombre_ = n; }

    Genero genero() const                        { return genero_; }
    void   establecerGenero(Genero g)            { genero_ = g; }

    /// Nombre base de su hoja de sprites: "conejo_macho", "castor_hembra".
    /// Vive en la capa logica porque es solo texto; quien lo convierte en una
    /// ruta de archivo es la capa grafica.
    std::string claveArte() const;

    /**
     * @brief Vocal de concordancia: "o" para macho, "a" para hembra.
     *
     * Los mensajes de la bitacora hablan de la mascota por su nombre, asi que
     * los adjetivos tienen que concordar con su sexo. Se escriben cortados:
     * `nombre() + " esta cansad" + terminacion() + "."`. Es feo de leer en el
     * codigo, pero es la unica forma de que el texto salga bien sin duplicar
     * cada frase.
     */
    std::string terminacion() const;

    /// Pronombre de objeto directo: "lo" o "la". Para frases como "banarlo".
    std::string pronombre() const;

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

    /// Arranca una actividad que ira subiendo un atributo poco a poco.
    bool iniciarActividad(Actividad actividad, float total, float segundos,
                          const std::string& mensajeInicio);

    /// Entrega la parte de la actividad que toca a este fotograma.
    void avanzarActividad(float dt);

    /// El atributo que sube la actividad en curso, o nullptr si ninguno.
    Atributo* atributoDeActividad();

    std::string nombre_;
    Genero      genero_       = Genero::Macho;
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

    // --- Actividad en curso -------------------------------------------------
    // La actividad entrega `ritmoActividad_` puntos por segundo hasta agotar
    // `restanteActividad_`. Se guarda tambien el total para poder decir cuanto
    // lleva hecho, que es lo que dibuja la metrica del Admin_Menu.
    Actividad   actividadEnCurso_  = Actividad::Ninguna;
    float       restanteActividad_ = 0.f;
    float       totalActividad_    = 0.f;
    float       ritmoActividad_    = 0.f;
    std::string objetoActividad_;

    static constexpr std::size_t kMaxBitacora = 20;
};

} // namespace vp
