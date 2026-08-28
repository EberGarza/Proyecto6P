#pragma once

#include "Estado.hpp"

namespace vp {

/// Estado por defecto: la mascota esta tranquila y sin necesidades urgentes.
class EstadoNormal : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Normal; }
    std::string mensaje() const override;
};

/// Felicidad alta y ninguna necesidad critica.
class EstadoFeliz : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Feliz; }
    std::string mensaje() const override;
};

/// Saciedad baja: la salud empieza a resentirse hasta que se le da de comer.
class EstadoHambrienta : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Hambrienta; }
    std::string mensaje() const override;
};

/// Energia baja: si llega a cero, la mascota se duerme sola.
class EstadoCansada : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Cansada; }
    std::string mensaje() const override;
};

/// Recupera energia rapidamente. Bloquea las interacciones del jugador.
class EstadoDurmiendo : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    void alSalir(Mascota& mascota) override;
    TipoEstado tipo() const override        { return TipoEstado::Durmiendo; }
    std::string mensaje() const override;
    bool permiteInteraccion() const override { return false; }

private:
    static constexpr float kRecuperacionEnergia = 8.0f;  ///< puntos por segundo
};

/// Salud baja: solo se recupera con medicina.
class EstadoEnferma : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    void alSalir(Mascota& mascota) override;
    TipoEstado tipo() const override    { return TipoEstado::Enferma; }
    std::string mensaje() const override;

private:
    static constexpr float kCastigoFelicidad = 1.5f;     ///< puntos por segundo
};

/// Estado temporal con duracion: al terminar vuelve a Normal o Feliz.
class EstadoJugando : public Estado
{
public:
    explicit EstadoJugando(float duracion = 5.f);

    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    void alSalir(Mascota& mascota) override;
    TipoEstado tipo() const override    { return TipoEstado::Jugando; }
    std::string mensaje() const override;

    float restante() const { return restante_; }

private:
    float duracion_;
    float restante_;
};

/// Estado terminal: se congela todo y no se acepta ninguna interaccion.
class EstadoMuerta : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override        { return TipoEstado::Muerta; }
    std::string mensaje() const override;
    bool permiteInteraccion() const override { return false; }
};

} // namespace vp
