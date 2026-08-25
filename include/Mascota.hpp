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

class Mascota
{
public:
    Mascota(std::string nombre, Genero genero);
    virtual ~Mascota() = default;

    Mascota(const Mascota&)            = delete;
    Mascota& operator=(const Mascota&) = delete;

    void actualizar(float dt);

    bool alimentar(float cantidad, const std::string& queCome = "su comida");
    bool jugar(float intensidad, const std::string& conQue = "");
    bool asear(float cantidad, const std::string& conQue = "");
    bool medicar(float cantidad, const std::string& conQue = "su medicina");
    bool dormir();
    bool despertar();
    bool acariciar();

    Permiso puede(AccionMascota accion) const;

    Actividad actividad() const;

    bool ocupada() const;

    float progresoActividad() const;

    void cancelarActividad();

protected:

    bool iniciarAccionEspecial(float animo, float segundos, const std::string& mensaje);

public:

    void revivir(float saludInicial = 50.f);

    const std::string& nombre() const            { return nombre_; }
    void establecerNombre(const std::string& n)  { nombre_ = n; }

    Genero genero() const                        { return genero_; }
    void   establecerGenero(Genero g)            { genero_ = g; }

    std::string claveArte() const;

    std::string terminacion() const;

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

    const Estado&   estado() const;
    TipoEstado      tipoEstado() const;
    MaquinaEstados& maquina()             { return maquina_; }
    void            cambiarEstado(TipoEstado nuevo);

    void registrar(const std::string& mensaje);
    const std::deque<std::string>& bitacora() const { return bitacora_; }
    std::string ultimoEvento() const;

    void restaurarTasasBase();

    float tasaBaseSaciedad() const  { return tasaBaseSaciedad_; }
    float tasaBaseFelicidad() const { return tasaBaseFelicidad_; }
    float tasaBaseEnergia() const   { return tasaBaseEnergia_; }
    float tasaBaseHigiene() const   { return tasaBaseHigiene_; }

    virtual std::string especie() const        = 0;
    virtual std::string sonido() const         = 0;
    virtual std::string comidaFavorita() const = 0;

    virtual void configurarRasgos() = 0;

    virtual std::string descripcion() const;

protected:

    void establecerTasasBase(float saciedad, float felicidad,
                             float energia, float higiene);

    virtual void alCambiarEstado(TipoEstado anterior, TipoEstado nuevo);

private:

    void aplicarEfectosSecundarios(float dt);

    bool iniciarActividad(Actividad actividad, float total, float segundos,
                          const std::string& mensajeInicio);

    void avanzarActividad(float dt);

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

    Actividad   actividadEnCurso_  = Actividad::Ninguna;
    float       restanteActividad_ = 0.f;
    float       totalActividad_    = 0.f;
    float       ritmoActividad_    = 0.f;
    std::string objetoActividad_;

    static constexpr std::size_t kMaxBitacora = 20;
};

}
