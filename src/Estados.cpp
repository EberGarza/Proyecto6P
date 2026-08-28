#include "Estados.hpp"

#include "Mascota.hpp"
#include "Utilidades.hpp"

namespace vp {

// ---------------------------------------------------------------- Normal ---

void EstadoNormal::alEntrar(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " esta tranquila.");
}

void EstadoNormal::actualizar(Mascota& mascota, float)
{
    if (auto critico = transicionCritica(mascota))
    {
        mascota.cambiarEstado(*critico);
        return;
    }

    if (mascota.felicidad().porEncimaDe(kUmbralFelicidad))
        mascota.cambiarEstado(TipoEstado::Feliz);
}

std::string EstadoNormal::mensaje() const
{
    return "Todo tranquilo por aqui.";
}

// ----------------------------------------------------------------- Feliz ---

void EstadoFeliz::alEntrar(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " esta feliz. " + mascota.sonido());
}

void EstadoFeliz::actualizar(Mascota& mascota, float)
{
    if (auto critico = transicionCritica(mascota))
    {
        mascota.cambiarEstado(*critico);
        return;
    }

    // Sale de Feliz con un umbral mas bajo del que necesito para entrar.
    // Esta histeresis evita que el estado parpadee entre Normal y Feliz.
    if (mascota.felicidad().porDebajoDe(kUmbralFelicidad - 20.f))
        mascota.cambiarEstado(TipoEstado::Normal);
}

std::string EstadoFeliz::mensaje() const
{
    return "Esta dando saltos de alegria.";
}

// ------------------------------------------------------------ Hambrienta ---

void EstadoHambrienta::alEntrar(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " tiene hambre. Quiere " +
                      mascota.comidaFavorita() + ".");
}

void EstadoHambrienta::actualizar(Mascota& mascota, float dt)
{
    if (!mascota.estaViva())
    {
        mascota.cambiarEstado(TipoEstado::Muerta);
        return;
    }

    // El hambre prolongada desgasta el animo.
    mascota.felicidad().modificar(-0.8f * dt);

    if (mascota.salud().porDebajoDe(kUmbralEnfermedad))
    {
        mascota.cambiarEstado(TipoEstado::Enferma);
        return;
    }

    if (mascota.saciedad().porEncimaDe(kUmbralHambre + 20.f))
        mascota.cambiarEstado(TipoEstado::Normal);
}

std::string EstadoHambrienta::mensaje() const
{
    return "Le ruge el estomago...";
}

// --------------------------------------------------------------- Cansada ---

void EstadoCansada::alEntrar(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " esta cansada.");
}

void EstadoCansada::actualizar(Mascota& mascota, float)
{
    if (!mascota.estaViva())
    {
        mascota.cambiarEstado(TipoEstado::Muerta);
        return;
    }

    // Sin energia se desmaya y pasa a dormir aunque el jugador no lo pida.
    if (mascota.energia().enMinimo())
    {
        mascota.registrar(mascota.nombre() + " no aguanto mas y se desplomo.");
        mascota.cambiarEstado(TipoEstado::Durmiendo);
        return;
    }

    if (mascota.salud().porDebajoDe(kUmbralEnfermedad))
    {
        mascota.cambiarEstado(TipoEstado::Enferma);
        return;
    }

    if (mascota.saciedad().porDebajoDe(kUmbralHambre))
    {
        mascota.cambiarEstado(TipoEstado::Hambrienta);
        return;
    }

    if (mascota.energia().porEncimaDe(kUmbralCansancio + 15.f))
        mascota.cambiarEstado(TipoEstado::Normal);
}

std::string EstadoCansada::mensaje() const
{
    return "Apenas puede mantener los ojos abiertos.";
}

// ------------------------------------------------------------- Durmiendo ---

void EstadoDurmiendo::alEntrar(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " se durmio. Zzz...");

    // Mientras duerme cambian las tasas: recupera energia y gasta menos.
    mascota.energia().establecerTasa(kRecuperacionEnergia);
    mascota.saciedad().establecerTasa(mascota.tasaBaseSaciedad() * 0.3f);
    mascota.higiene().establecerTasa(0.f);
}

void EstadoDurmiendo::actualizar(Mascota& mascota, float dt)
{
    if (!mascota.estaViva())
    {
        mascota.cambiarEstado(TipoEstado::Muerta);
        return;
    }

    // Dormir tambien repone un poco de salud.
    mascota.salud().modificar(0.4f * dt);

    if (mascota.energia().enMaximo())
        mascota.cambiarEstado(TipoEstado::Normal);
}

void EstadoDurmiendo::alSalir(Mascota& mascota)
{
    mascota.restaurarTasasBase();
    mascota.registrar(mascota.nombre() + " desperto.");
}

std::string EstadoDurmiendo::mensaje() const
{
    return "Zzz... no la despiertes.";
}

// --------------------------------------------------------------- Enferma ---

void EstadoEnferma::alEntrar(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " se enfermo. Necesita medicina.");
}

void EstadoEnferma::actualizar(Mascota& mascota, float dt)
{
    if (!mascota.estaViva())
    {
        mascota.cambiarEstado(TipoEstado::Muerta);
        return;
    }

    mascota.felicidad().modificar(-kCastigoFelicidad * dt);

    // Solo se sale de aqui subiendo la salud (medicina o descanso largo).
    if (mascota.salud().porEncimaDe(kUmbralEnfermedad + 20.f))
        mascota.cambiarEstado(TipoEstado::Normal);
}

void EstadoEnferma::alSalir(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " se recupero.");
}

std::string EstadoEnferma::mensaje() const
{
    return "No se siente nada bien...";
}

// --------------------------------------------------------------- Jugando ---

EstadoJugando::EstadoJugando(float duracion)
    : duracion_(duracion)
    , restante_(duracion)
{
}

void EstadoJugando::alEntrar(Mascota& mascota)
{
    restante_ = duracion_;
    mascota.registrar(mascota.nombre() + " se puso a jugar. " + mascota.sonido());
}

void EstadoJugando::actualizar(Mascota& mascota, float dt)
{
    if (!mascota.estaViva())
    {
        mascota.cambiarEstado(TipoEstado::Muerta);
        return;
    }

    restante_ -= dt;

    // Jugar sube el animo pero cuesta energia y ensucia.
    mascota.felicidad().modificar(3.0f * dt);
    mascota.energia().modificar(-2.0f * dt);
    mascota.higiene().modificar(-1.5f * dt);

    if (restante_ <= 0.f)
    {
        if (auto critico = transicionCritica(mascota))
            mascota.cambiarEstado(*critico);
        else
            mascota.cambiarEstado(TipoEstado::Feliz);
    }
}

void EstadoJugando::alSalir(Mascota& mascota)
{
    mascota.registrar(mascota.nombre() + " termino de jugar.");
}

std::string EstadoJugando::mensaje() const
{
    return "Esta jugando sin parar.";
}

// ---------------------------------------------------------------- Muerta ---

void EstadoMuerta::alEntrar(Mascota& mascota)
{
    // Se congelan todos los cambios automaticos.
    mascota.saciedad().establecerTasa(0.f);
    mascota.felicidad().establecerTasa(0.f);
    mascota.energia().establecerTasa(0.f);
    mascota.higiene().establecerTasa(0.f);
    mascota.registrar(mascota.nombre() + " nos dejo. Descansa en paz.");
}

void EstadoMuerta::actualizar(Mascota&, float)
{
    // Estado terminal: no hace nada y no transita a ningun otro estado.
}

std::string EstadoMuerta::mensaje() const
{
    return "Ya no esta con nosotros.";
}

} // namespace vp
