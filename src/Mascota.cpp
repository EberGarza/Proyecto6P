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

    establecerTasasBase(-0.14f, -0.10f, -0.10f, -0.09f);
    maquina_.iniciar(*this, TipoEstado::Normal);
}

void Mascota::actualizar(float dt)
{
    if (!viva_) return;
    if (dt < 0.f) return;

    edadSegundos_ += dt;

    saciedad_.actualizar(dt);
    felicidad_.actualizar(dt);
    energia_.actualizar(dt);
    higiene_.actualizar(dt);

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

    if (saciedad_.porDebajoDe(15.f))  cambioSalud -= 0.25f;
    if (higiene_.porDebajoDe(20.f))   cambioSalud -= 0.18f;
    if (felicidad_.porDebajoDe(10.f)) cambioSalud -= 0.10f;

    const bool bienCuidada = saciedad_.porEncimaDe(60.f) &&
                             higiene_.porEncimaDe(60.f) &&
                             energia_.porEncimaDe(40.f);
    if (cambioSalud == 0.f && bienCuidada) cambioSalud = 0.15f;

    salud_.modificar(cambioSalud * dt);
}

Actividad Mascota::actividad() const
{

    if (tipoEstado() == TipoEstado::Durmiendo) return Actividad::Durmiendo;
    if (tipoEstado() == TipoEstado::Jugando)   return Actividad::Jugando;
    return actividadEnCurso_;
}

bool Mascota::ocupada() const
{
    return actividad() != Actividad::Ninguna;
}

Permiso Mascota::puede(AccionMascota accion) const
{
    const std::string ella = pronombre();

    if (!viva_)
        return { false, nombre_ + " ya no esta." };

    if (tipoEstado() == TipoEstado::Durmiendo)
    {
        if (accion == AccionMascota::Despertar) return { true, {} };
        return { false, nombre_ + " esta durmiendo. Despierta" + ella + " primero." };
    }

    if (accion == AccionMascota::Despertar)
        return { false, nombre_ + " ya esta despiert" + terminacion() + "." };

    if (ocupada())
    {
        const std::string haciendo = util::aMinusculas(nombreActividad(actividad()));
        return { false, nombre_ + " esta " + haciendo + ". Espera a que termine." };
    }

    switch (accion)
    {
        case AccionMascota::Alimentar:
            if (saciedad_.enMaximo())
                return { false, nombre_ + " ya esta llen" + terminacion() + ", no quiere mas." };
            break;

        case AccionMascota::Asear:
            if (higiene_.enMaximo())
                return { false, nombre_ + " ya esta impecable." };
            break;

        case AccionMascota::Medicar:
            if (salud_.enMaximo())
                return { false, nombre_ + " esta san" + terminacion() + ", no necesita medicina." };
            break;

        case AccionMascota::Jugar:
            if (energia_.porDebajoDe(15.f))
                return { false, nombre_ + " esta demasiado cansad" + terminacion() + " para jugar." };
            break;

        case AccionMascota::Especial:
            if (energia_.porDebajoDe(20.f))
                return { false, nombre_ + " no tiene energia para eso." };
            break;

        case AccionMascota::Dormir:
        case AccionMascota::Acariciar:
        case AccionMascota::Despertar:
            break;
    }

    return { true, {} };
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
        case Actividad::Comiendo:    return &saciedad_;
        case Actividad::Jugando:     return &felicidad_;
        case Actividad::Aseandose:   return &higiene_;
        case Actividad::Curandose:   return &salud_;
        case Actividad::Acariciando: return &felicidad_;
        case Actividad::Presumiendo: return &felicidad_;

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

    const Actividad terminada = actividadEnCurso_;
    const std::string queEra  = objetoActividad_;
    actividadEnCurso_ = Actividad::Ninguna;
    objetoActividad_.clear();

    switch (terminada)
    {
        case Actividad::Comiendo:
            felicidad_.modificar(totalActividad_ * 0.15f);
            higiene_.modificar(-totalActividad_ * 0.10f);
            registrar(nombre_ + " termino de comer " + queEra + ". " + sonido());
            break;

        case Actividad::Aseandose:
            felicidad_.modificar(-totalActividad_ * 0.10f);
            registrar(nombre_ + " quedo limpi" + terminacion() + ".");
            break;

        case Actividad::Curandose:
            felicidad_.modificar(-totalActividad_ * 0.20f);
            registrar(nombre_ + " se tomo " + queEra + ".");
            break;

        case Actividad::Acariciando:
            registrar(nombre_ + " se queda pidiendo mas mimos.");
            break;

        case Actividad::Presumiendo:

            break;

        case Actividad::Jugando:

            break;

        case Actividad::Durmiendo:
        case Actividad::Ninguna:
            break;
    }
}

bool Mascota::iniciarAccionEspecial(float animo, float segundos,
                                    const std::string& mensaje)
{
    return iniciarActividad(Actividad::Presumiendo, animo, segundos, mensaje);
}

void Mascota::cancelarActividad()
{
    actividadEnCurso_  = Actividad::Ninguna;
    restanteActividad_ = 0.f;
    totalActividad_    = 0.f;
    objetoActividad_.clear();
}

bool Mascota::alimentar(float cantidad, const std::string& queCome)
{
    const Permiso permiso = puede(AccionMascota::Alimentar);
    if (!permiso) { registrar(permiso.motivo); return false; }

    objetoActividad_ = queCome;
    return iniciarActividad(Actividad::Comiendo, cantidad, cantidad / 8.f,
                            nombre_ + " se puso a comer " + queCome + ".");
}

bool Mascota::jugar(float intensidad, const std::string& conQue)
{
    const Permiso permiso = puede(AccionMascota::Jugar);
    if (!permiso) { registrar(permiso.motivo); return false; }

    objetoActividad_ = conQue;
    iniciarActividad(Actividad::Jugando, intensidad, intensidad / 6.f, "");
    cambiarEstado(TipoEstado::Jugando);
    return true;
}

bool Mascota::asear(float cantidad, const std::string& conQue)
{
    const Permiso permiso = puede(AccionMascota::Asear);
    if (!permiso) { registrar(permiso.motivo); return false; }

    objetoActividad_ = conQue;
    const std::string con = conQue.empty() ? "" : " con " + conQue;
    return iniciarActividad(Actividad::Aseandose, cantidad, cantidad / 12.f,
                            nombre_ + " se esta banando" + con + ".");
}

bool Mascota::medicar(float cantidad, const std::string& conQue)
{
    const Permiso permiso = puede(AccionMascota::Medicar);
    if (!permiso) { registrar(permiso.motivo); return false; }

    objetoActividad_ = conQue;
    return iniciarActividad(Actividad::Curandose, cantidad, cantidad / 15.f,
                            nombre_ + " esta tomando " + conQue + ".");
}

bool Mascota::dormir()
{
    const Permiso permiso = puede(AccionMascota::Dormir);
    if (!permiso) { registrar(permiso.motivo); return false; }

    cambiarEstado(TipoEstado::Durmiendo);
    return true;
}

bool Mascota::despertar()
{
    const Permiso permiso = puede(AccionMascota::Despertar);
    if (!permiso) { registrar(permiso.motivo); return false; }

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
    const Permiso permiso = puede(AccionMascota::Acariciar);
    if (!permiso) { registrar(permiso.motivo); return false; }

    return iniciarActividad(Actividad::Acariciando, 8.f, 0.9f,
                            "Acariciaste a " + nombre_ + ". " + sonido());
}

void Mascota::revivir(float saludInicial)
{
    viva_ = true;
    salud_.establecer(saludInicial <= 0.f ? 1.f : saludInicial);
    restaurarTasasBase();

    maquina_.cambiarA(*this, TipoEstado::Normal);
    registrar(nombre_ + " volvio a la vida.");
}

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

const Estado& Mascota::estado() const   { return maquina_.estadoActual(); }
TipoEstado    Mascota::tipoEstado() const { return maquina_.tipoActual(); }

void Mascota::cambiarEstado(TipoEstado nuevo)
{
    const TipoEstado anterior = maquina_.tipoActual();

    if (anterior == nuevo) return;
    if (anterior == TipoEstado::Muerta) return;

    maquina_.cambiarA(*this, nuevo);
    alCambiarEstado(anterior, nuevo);
}

void Mascota::alCambiarEstado(TipoEstado, TipoEstado)
{

}

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

}
