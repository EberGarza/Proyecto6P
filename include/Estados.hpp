#pragma once

#include "Estado.hpp"

namespace vp {

class EstadoNormal : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Normal; }
    std::string mensaje() const override;
};

class EstadoFeliz : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Feliz; }
    std::string mensaje() const override;
};

class EstadoHambrienta : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Hambrienta; }
    std::string mensaje() const override;
};

class EstadoCansada : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override    { return TipoEstado::Cansada; }
    std::string mensaje() const override;
};

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
    static constexpr float kRecuperacionEnergia = 4.0f;
};

class EstadoEnferma : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    void alSalir(Mascota& mascota) override;
    TipoEstado tipo() const override    { return TipoEstado::Enferma; }
    std::string mensaje() const override;

private:
    static constexpr float kCastigoFelicidad = 0.20f;
};

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

class EstadoMuerta : public Estado
{
public:
    void alEntrar(Mascota& mascota) override;
    void actualizar(Mascota& mascota, float dt) override;
    TipoEstado tipo() const override        { return TipoEstado::Muerta; }
    std::string mensaje() const override;
    bool permiteInteraccion() const override { return false; }
};

}
