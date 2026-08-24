#include "AdminMenu.hpp"

#include "Inventario.hpp"
#include "Mascota.hpp"
#include "Utilidades.hpp"

#include <cctype>
#include <cstring>

namespace vp {

// ----------------------------------------------------------------- Acceso ---

bool AdminMenu::registrarTecla(char tecla)
{
    if (desbloqueado_) return false;

    const char esperada = kSecuencia[avanceClave_];
    const char pulsada  = static_cast<char>(std::toupper(static_cast<unsigned char>(tecla)));

    if (pulsada == esperada)
    {
        ++avanceClave_;

        if (kSecuencia[avanceClave_] == '\0')
        {
            desbloqueado_ = true;
            visible_      = true;
            avanceClave_  = 0;
            anotar("Admin_Menu desbloqueado.");
            return true;
        }
        return false;
    }

    // Si falla, se reinicia; pero si la tecla coincide con la primera letra,
    // esa pulsacion ya cuenta como inicio de un intento nuevo.
    avanceClave_ = (pulsada == kSecuencia[0]) ? 1u : 0u;
    return false;
}

bool AdminMenu::intentarCodigo(const std::string& codigo)
{
    std::string mayusculas;
    mayusculas.reserve(codigo.size());
    for (char c : codigo)
        mayusculas += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    if (mayusculas != kSecuencia) return false;

    desbloqueado_ = true;
    visible_      = true;
    anotar("Admin_Menu desbloqueado.");
    return true;
}

void AdminMenu::alternarVisible()
{
    if (!desbloqueado_) return;
    visible_ = !visible_;
}

void AdminMenu::bloquear()
{
    desbloqueado_ = false;
    visible_      = false;
    avanceClave_  = 0;
    anotar("Admin_Menu bloqueado.");
}

// ------------------------------------------------ Comandos sobre la mascota -

void AdminMenu::forzarEstado(Mascota& mascota, TipoEstado tipo)
{
    // Muerta es terminal: para salir de ahi hay que revivir primero.
    if (mascota.tipoEstado() == TipoEstado::Muerta && tipo != TipoEstado::Muerta)
        mascota.revivir();

    if (tipo == TipoEstado::Muerta)
    {
        matar(mascota);
        return;
    }

    mascota.cambiarEstado(tipo);
    anotar("Estado forzado: " + nombreEstado(tipo));
}

void AdminMenu::rellenarAtributos(Mascota& mascota)
{
    mascota.saciedad().establecer(100.f);
    mascota.felicidad().establecer(100.f);
    mascota.energia().establecer(100.f);
    mascota.higiene().establecer(100.f);
    mascota.salud().establecer(100.f);
    anotar("Todas las barras al maximo.");
}

void AdminMenu::vaciarAtributos(Mascota& mascota)
{
    mascota.saciedad().establecer(0.f);
    mascota.felicidad().establecer(0.f);
    mascota.energia().establecer(0.f);
    mascota.higiene().establecer(0.f);
    // La salud se deja en 1 para poder observar la agonia sin matarla al instante.
    mascota.salud().establecer(1.f);
    anotar("Todas las barras al minimo.");
}

bool AdminMenu::establecerAtributo(Mascota& mascota, const std::string& clave, float valor)
{
    Atributo* destino = nullptr;

    if      (clave == "saciedad")  destino = &mascota.saciedad();
    else if (clave == "felicidad") destino = &mascota.felicidad();
    else if (clave == "energia")   destino = &mascota.energia();
    else if (clave == "higiene")   destino = &mascota.higiene();
    else if (clave == "salud")     destino = &mascota.salud();

    if (!destino) return false;

    destino->establecer(valor);
    anotar(clave + " = " + util::aTexto(valor));
    return true;
}

void AdminMenu::matar(Mascota& mascota)
{
    inmortalidad_ = false;
    mascota.salud().establecer(0.f);
    mascota.actualizar(0.f);   // dispara la comprobacion de muerte
    anotar("Mascota eliminada.");
}

void AdminMenu::revivir(Mascota& mascota)
{
    mascota.revivir(60.f);
    anotar("Mascota revivida.");
}

void AdminMenu::envejecer(Mascota& mascota, float dias)
{
    mascota.establecerEdad(mascota.edad() + dias * 86400.f);
    anotar("Edad +" + util::aTexto(dias) + " dia(s).");
}

// ------------------------------------------------------- Trampas activas ----

void AdminMenu::alternarInmortalidad()
{
    inmortalidad_ = !inmortalidad_;
    anotar(inmortalidad_ ? "Inmortalidad activada." : "Inmortalidad desactivada.");
}

void AdminMenu::alternarMetrica()
{
    metrica_ = !metrica_;
    anotar(metrica_ ? "Metrica de actividad encendida."
                    : "Metrica de actividad apagada.");
}

void AdminMenu::aplicarPorTick(Mascota& mascota, float)
{
    if (!inmortalidad_) return;

    // Se deja un piso de salud en lugar de fijarla en 100: asi la mascota
    // todavia puede entrar en Enferma y se ven las transiciones.
    if (mascota.salud().porDebajoDe(kSaludMinima))
        mascota.salud().establecer(kSaludMinima);
}

// ---------------------------------------------------- Velocidad del reloj ---

void AdminMenu::siguienteEscala()
{
    indiceEscala_ = (indiceEscala_ + 1) % kEscalas.size();
    anotar("Velocidad x" + util::aTexto(escalaTiempo()));
}

// ---------------------------------------------------------------- Economia --

void AdminMenu::agregarMonedas(Inventario& inventario, int cantidad)
{
    inventario.agregarMonedas(cantidad);
    anotar("+" + std::to_string(cantidad) + " monedas.");
}

// ---------------------------------------------------------------- Registro --

void AdminMenu::anotar(const std::string& mensaje)
{
    registro_.push_back(mensaje);
    while (registro_.size() > kMaxRegistro)
        registro_.pop_front();
}

std::string AdminMenu::ultimaAccion() const
{
    return registro_.empty() ? std::string() : registro_.back();
}

} // namespace vp
