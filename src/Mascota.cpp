#include "Mascota.hpp"

#include "Estado.hpp"
#include "Utilidades.hpp"

#include <algorithm>
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
    establecerTasasBase(-0.14f, -0.10f, -0.10f, -0.09f);
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

    // La actividad va antes que los efectos cruzados: si la mascota esta
    // comiendo, lo justo es que la saciedad de este fotograma cuente ya.
    avanzarActividad(dt);

    aplicarEfectosSecundarios(dt);
    maquina_.actualizar(*this, dt);

    if (salud_.enMinimo())
    {
        viva_ = false;
        cancelarActividad();
        cambiarEstado(TipoEstado::Muerta);
    }
}

void Mascota::aplicarEfectosSecundarios(float dt)
{
    float cambioSalud = 0.f;

    if (saciedad_.porDebajoDe(15.f))  cambioSalud -= 0.25f;   // desnutricion
    if (higiene_.porDebajoDe(20.f))   cambioSalud -= 0.18f;   // infecciones
    if (felicidad_.porDebajoDe(10.f)) cambioSalud -= 0.10f;   // depresion

    // Si todo esta bien, la mascota se recupera poco a poco.
    const bool bienCuidada = saciedad_.porEncimaDe(60.f) &&
                             higiene_.porEncimaDe(60.f) &&
                             energia_.porEncimaDe(40.f);
    if (cambioSalud == 0.f && bienCuidada) cambioSalud = 0.15f;

    salud_.modificar(cambioSalud * dt);
}

// ------------------------------------------------------------ Actividad -----

Actividad Mascota::actividad() const
{
    // Dormir no es una actividad con cuenta atras sino un estado, pero para
    // quien mira la pantalla es lo mismo: la mascota esta durmiendo.
    if (tipoEstado() == TipoEstado::Durmiendo) return Actividad::Durmiendo;
    return actividadEnCurso_;
}

float Mascota::progresoActividad() const
{
    if (actividadEnCurso_ == Actividad::Ninguna || totalActividad_ <= 0.f) return 0.f;
    return 1.f - (restanteActividad_ / totalActividad_);
}

Atributo* Mascota::atributoDeActividad()
{
    switch (actividadEnCurso_)
    {
        case Actividad::Comiendo:  return &saciedad_;
        case Actividad::Jugando:   return &felicidad_;
        case Actividad::Aseandose: return &higiene_;
        case Actividad::Curandose: return &salud_;

        case Actividad::Durmiendo:
        case Actividad::Ninguna:
            break;
    }
    return nullptr;
}

bool Mascota::iniciarActividad(Actividad actividad, float total, float segundos,
                               const std::string& mensajeInicio)
{
    if (total <= 0.f || segundos <= 0.f) return false;

    actividadEnCurso_  = actividad;
    totalActividad_    = total;
    restanteActividad_ = total;
    ritmoActividad_    = total / segundos;

    if (!mensajeInicio.empty()) registrar(mensajeInicio);
    return true;
}

void Mascota::avanzarActividad(float dt)
{
    if (actividadEnCurso_ == Actividad::Ninguna) return;

    Atributo* destino = atributoDeActividad();
    if (!destino) { cancelarActividad(); return; }

    // Si la barra ya esta llena no tiene sentido seguir: se corta y se avisa,
    // en vez de dejar la animacion repitiendose sin efecto.
    if (destino->enMaximo())
    {
        const Actividad terminada = actividadEnCurso_;
        actividadEnCurso_ = Actividad::Ninguna;

        if (terminada == Actividad::Comiendo)
            registrar(nombre_ + " ya no puede mas.");
        return;
    }

    const float paso = std::min(restanteActividad_, ritmoActividad_ * dt);
    destino->modificar(paso);
    restanteActividad_ -= paso;

    if (restanteActividad_ > 0.f) return;

    // --- Terminada: efectos secundarios y mensaje de cierre ------------------
    const Actividad terminada = actividadEnCurso_;
    const std::string queEra  = objetoActividad_;
    actividadEnCurso_ = Actividad::Ninguna;
    objetoActividad_.clear();

    switch (terminada)
    {
        case Actividad::Comiendo:
            felicidad_.modificar(totalActividad_ * 0.15f);
            higiene_.modificar(-totalActividad_ * 0.10f);   // comer ensucia
            registrar(nombre_ + " termino de comer " + queEra + ". " + sonido());
            break;

        case Actividad::Aseandose:
            felicidad_.modificar(-totalActividad_ * 0.10f);  // el bano no le encanta
            registrar(nombre_ + " quedo limpi" + terminacion() + ".");
            break;

        case Actividad::Curandose:
            felicidad_.modificar(-totalActividad_ * 0.20f);  // la medicina sabe feo
            registrar(nombre_ + " se tomo " + queEra + ".");
            break;

        case Actividad::Jugando:
            // El estado Jugando se encarga del resto y del mensaje de salida.
            break;

        case Actividad::Durmiendo:
        case Actividad::Ninguna:
            break;
    }
}

void Mascota::cancelarActividad()
{
    actividadEnCurso_  = Actividad::Ninguna;
    restanteActividad_ = 0.f;
    totalActividad_    = 0.f;
    objetoActividad_.clear();
}

// ------------------------------------------------------------- Acciones -----

bool Mascota::alimentar(float cantidad, const std::string& queCome)
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion())
    {
        registrar(nombre_ + " no puede comer ahora mismo.");
        return false;
    }
    if (ocupada())
    {
        registrar(nombre_ + " esta ocupad" + terminacion() + " todavia.");
        return false;
    }
    if (saciedad_.enMaximo())
    {
        registrar(nombre_ + " ya esta llen" + terminacion() + ", no quiere mas.");
        return false;
    }

    // Un bocado por segundo, mas o menos: comer 40 puntos lleva unos 5 s. Asi
    // la animacion de comer se ve y la barra sube a la vista.
    objetoActividad_ = queCome;
    return iniciarActividad(Actividad::Comiendo, cantidad, cantidad / 8.f,
                            nombre_ + " se puso a comer " + queCome + ".");
}

bool Mascota::jugar(float intensidad, const std::string& conQue)
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion())
    {
        registrar(nombre_ + " no esta para juegos ahora.");
        return false;
    }
    if (ocupada())
    {
        registrar(nombre_ + " esta ocupad" + terminacion() + " todavia.");
        return false;
    }
    if (energia_.porDebajoDe(15.f))
    {
        registrar(nombre_ + " esta demasiado cansad" + terminacion() + " para jugar.");
        return false;
    }

    // Jugar tiene ademas su propio estado, que es el que aplica el desgaste de
    // energia mientras dura. La actividad solo lleva la cuenta del animo.
    objetoActividad_ = conQue;
    iniciarActividad(Actividad::Jugando, intensidad, intensidad / 6.f, "");
    cambiarEstado(TipoEstado::Jugando);
    return true;
}

bool Mascota::asear(float cantidad, const std::string& conQue)
{
    if (!viva_) return false;
    if (!estado().permiteInteraccion())
    {
        registrar("Mejor no banar" + pronombre() + " mientras duerme.");
        return false;
    }
    if (ocupada())
    {
        registrar(nombre_ + " esta ocupad" + terminacion() + " todavia.");
        return false;
    }
    if (higiene_.enMaximo())
    {
        registrar(nombre_ + " ya esta impecable.");
        return false;
    }

    objetoActividad_ = conQue;
    const std::string con = conQue.empty() ? "" : " con " + conQue;
    return iniciarActividad(Actividad::Aseandose, cantidad, cantidad / 12.f,
                            nombre_ + " se esta banando" + con + ".");
}

bool Mascota::medicar(float cantidad, const std::string& conQue)
{
    if (!viva_) return false;
    if (ocupada())
    {
        registrar(nombre_ + " esta ocupad" + terminacion() + " todavia.");
        return false;
    }
    if (salud_.enMaximo())
    {
        registrar(nombre_ + " esta san" + terminacion() + ", no necesita medicina.");
        return false;
    }

    objetoActividad_ = conQue;
    return iniciarActividad(Actividad::Curandose, cantidad, cantidad / 15.f,
                            nombre_ + " esta tomando " + conQue + ".");
}

bool Mascota::dormir()
{
    if (!viva_) return false;
    if (tipoEstado() == TipoEstado::Durmiendo)
    {
        registrar(nombre_ + " ya esta dormid" + terminacion() + ".");
        return false;
    }

    // Dormirse interrumpe lo que estuviera haciendo.
    cancelarActividad();
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
