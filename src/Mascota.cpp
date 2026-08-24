#include "Mascota.hpp"

#include "Estado.hpp"
#include "Utilidades.hpp"

#include <cctype>
#include <utility>

namespace vp {

Mascota::Mascota(std::string nombre, Genero genero)
    : nombre_(std::move(nombre))
    , genero_(genero)
    , saciedad_ ("Saciedad",  100.f)
    , felicidad_("Felicidad",  80.f)
    , energia_  ("Energia",   100.f)
    , higiene_  ("Higiene",   100.f)
    , salud_    ("Salud",     100.f)
{
    // Valores neutros por defecto; cada especie los sobreescribe en su
    // constructor llamando a configurarRasgos().
    establecerTasasBase(-1.0f, -0.6f, -0.8f, -0.5f);
    maquina_.iniciar(*this, TipoEstado::Normal);
}

// ----------------------------------------------------------- Ciclo de vida --

void Mascota::actualizar(float dt)
{
    if (!viva_) return;
    if (dt < 0.f) return;

    edadSegundos_ += dt;

    saciedad_.actualizar(dt);
    felicidad_.actualizar(dt);
    energia_.actualizar(dt);
    higiene_.actualizar(dt);
    // La salud no decae sola: depende del resto de atributos.

    aplicarEfectosSecundarios(dt);
    maquina_.actualizar(*this, dt);

    if (salud_.enMinimo())
    {
        viva_ = false;
        cambiarEstado(TipoEstado::Muerta);
    }
}

void Mascota::aplicarEfectosSecundarios(float dt)
{
    float cambioSalud = 0.f;

    if (saciedad_.porDebajoDe(15.f))  cambioSalud -= 1.5f;   // desnutricion
    if (higiene_.porDebajoDe(20.f))   cambioSalud -= 1.0f;   // infecciones
    if (felicidad_.porDebajoDe(10.f)) cambioSalud -= 0.5f;   // depresion

    // Si todo esta bien, la mascota se recupera poco a poco.
    const bool bienCuidada = saciedad_.porEncimaDe(60.f) &&
                             higiene_.porEncimaDe(60.f) &&
                             energia_.porEncimaDe(40.f);
    if (cambioSalud == 0.f && bienCuidada) cambioSalud = 0.8f;

    salud_.modificar(cambioSalud * dt);
}

// ------------------------------------------------------------- Acciones -----

bool Mascota::alimentar(float cantidad)
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion())
    {
        registrar(nombre_ + " no puede comer ahora mismo.");
        return false;
    }
    if (saciedad_.enMaximo())
    {
        registrar(nombre_ + " ya esta llen" + terminacion() + ", no quiere mas.");
        return false;
    }

    saciedad_.modificar(cantidad);
    felicidad_.modificar(cantidad * 0.15f);
    higiene_.modificar(-cantidad * 0.10f);   // comer ensucia un poco

    registrar(nombre_ + " comio (+" + util::aTexto(cantidad) + " saciedad). " + sonido());
    return true;
}

bool Mascota::jugar(float intensidad)
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion())
    {
        registrar(nombre_ + " no esta para juegos ahora.");
        return false;
    }
    if (energia_.porDebajoDe(15.f))
    {
        registrar(nombre_ + " esta demasiado cansad" + terminacion() + " para jugar.");
        return false;
    }

    felicidad_.modificar(intensidad);
    energia_.modificar(-intensidad * 0.6f);
    saciedad_.modificar(-intensidad * 0.3f);

    cambiarEstado(TipoEstado::Jugando);
    return true;
}

bool Mascota::asear(float cantidad)
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion())
    {
        registrar("Mejor no banar" + pronombre() + " mientras duerme.");
        return false;
    }
    if (higiene_.enMaximo())
    {
        registrar(nombre_ + " ya esta impecable.");
        return false;
    }

    higiene_.modificar(cantidad);
    felicidad_.modificar(-cantidad * 0.10f);   // el bano no le encanta

    registrar(nombre_ + " quedo limpi" + terminacion() + " (+" + util::aTexto(cantidad) + " higiene).");
    return true;
}

bool Mascota::medicar(float cantidad)
{
    if (!viva_) return false;
    if (salud_.enMaximo())
    {
        registrar(nombre_ + " esta san" + terminacion() + ", no necesita medicina.");
        return false;
    }

    salud_.modificar(cantidad);
    felicidad_.modificar(-cantidad * 0.20f);   // la medicina sabe feo

    registrar(nombre_ + " tomo su medicina (+" + util::aTexto(cantidad) + " salud).");
    return true;
}

bool Mascota::dormir()
{
    if (!viva_) return false;
    if (tipoEstado() == TipoEstado::Durmiendo)
    {
        registrar(nombre_ + " ya esta dormid" + terminacion() + ".");
        return false;
    }

    cambiarEstado(TipoEstado::Durmiendo);
    return true;
}

bool Mascota::despertar()
{
    if (!viva_) return false;
    if (tipoEstado() != TipoEstado::Durmiendo)
    {
        registrar(nombre_ + " ya esta despiert" + terminacion() + ".");
        return false;
    }

    // Despertarla antes de tiempo le molesta.
    if (!energia_.enMaximo())
    {
        felicidad_.modificar(-8.f);
        registrar("Despertaste a " + nombre_ + " antes de tiempo.");
    }

    cambiarEstado(TipoEstado::Normal);
    return true;
}

bool Mascota::acariciar()
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion()) return false;

    felicidad_.modificar(5.f);
    registrar("Acariciaste a " + nombre_ + ". " + sonido());
    return true;
}

void Mascota::revivir(float saludInicial)
{
    viva_ = true;
    salud_.establecer(saludInicial <= 0.f ? 1.f : saludInicial);
    restaurarTasasBase();

    // Muerta es un estado terminal, asi que cambiarEstado() ignoraria la
    // peticion: hay que pedirle el cambio directamente a la maquina.
    maquina_.cambiarA(*this, TipoEstado::Normal);
    registrar(nombre_ + " volvio a la vida.");
}

// ------------------------------------------------------------ Consultas -----

int Mascota::edadEnDias() const
{
    return static_cast<int>(edadSegundos_ / 86400.f);
}

bool Mascota::tieneHambre() const  { return saciedad_.porDebajoDe(30.f); }
bool Mascota::estaCansada() const  { return energia_.porDebajoDe(30.f); }
bool Mascota::estaSucia() const    { return higiene_.porDebajoDe(30.f); }
bool Mascota::estaEnferma() const  { return salud_.porDebajoDe(30.f); }

std::string Mascota::descripcion() const
{
    return nombre_ + ", " + especie() + " " + nombreGenero(genero_)
         + ". Le encanta " + comidaFavorita() + ".";
}

std::string Mascota::terminacion() const
{
    return (genero_ == Genero::Macho) ? "o" : "a";
}

std::string Mascota::pronombre() const
{
    return (genero_ == Genero::Macho) ? "lo" : "la";
}

std::string Mascota::claveArte() const
{
    std::string clave = especie();
    for (char& c : clave)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    return clave + "_" + sufijoGenero(genero_);
}

// -------------------------------------------------------------- Estados -----

const Estado& Mascota::estado() const   { return maquina_.estadoActual(); }
TipoEstado    Mascota::tipoEstado() const { return maquina_.tipoActual(); }

void Mascota::cambiarEstado(TipoEstado nuevo)
{
    const TipoEstado anterior = maquina_.tipoActual();

    if (anterior == nuevo) return;
    if (anterior == TipoEstado::Muerta) return;   // estado terminal

    maquina_.cambiarA(*this, nuevo);
    alCambiarEstado(anterior, nuevo);
}

void Mascota::alCambiarEstado(TipoEstado, TipoEstado)
{
    // Gancho vacio: las especies concretas pueden reaccionar aqui.
}

// ------------------------------------------------------------- Bitacora -----

void Mascota::registrar(const std::string& mensaje)
{
    bitacora_.push_back(mensaje);
    while (bitacora_.size() > kMaxBitacora)
        bitacora_.pop_front();
}

std::string Mascota::ultimoEvento() const
{
    return bitacora_.empty() ? std::string() : bitacora_.back();
}

// ----------------------------------------------------------- Tasas base -----

void Mascota::establecerTasasBase(float saciedad, float felicidad,
                                  float energia, float higiene)
{
    tasaBaseSaciedad_  = saciedad;
    tasaBaseFelicidad_ = felicidad;
    tasaBaseEnergia_   = energia;
    tasaBaseHigiene_   = higiene;
    restaurarTasasBase();
}

void Mascota::restaurarTasasBase()
{
    saciedad_.establecerTasa(tasaBaseSaciedad_);
    felicidad_.establecerTasa(tasaBaseFelicidad_);
    energia_.establecerTasa(tasaBaseEnergia_);
    higiene_.establecerTasa(tasaBaseHigiene_);
}

} // namespace vp
