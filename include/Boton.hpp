#pragma once

#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <functional>
#include <string>
#include <utility>

namespace vp {

class Boton : public sf::Drawable
{
public:
    using Accion = std::function<void()>;

    Boton(const sf::Font& fuente, std::string etiqueta,
          sf::Vector2f posicion, sf::Vector2f tamano, Accion accion = nullptr);

    void actualizar(sf::Vector2f posicionRaton);

    bool procesarClic(sf::Vector2f posicionClic);

    bool accionar();

    void establecerAccion(Accion accion)  { accion_ = std::move(accion); }

    void establecerHabilitado(bool habilitado);

    void establecerDisponible(bool disponible);

    void establecerEtiqueta(const std::string& etiqueta);
    void establecerColores(sf::Color base, sf::Color resaltado);
    void establecerActivo(bool activo);
    void establecerTamanoTexto(unsigned tamano);

    void establecerTecla(const std::string& etiquetaTecla);

    bool habilitado() const { return habilitado_; }
    bool activo() const     { return activo_; }
    bool contiene(sf::Vector2f punto) const;

    sf::Vector2f posicion() const { return forma_.getPosition(); }
    sf::Vector2f tamano() const   { return forma_.getSize(); }

private:
    void draw(sf::RenderTarget& objetivo, sf::RenderStates estados) const override;
    void refrescarColor();
    void centrarTexto();

    sf::RectangleShape forma_;
    sf::RectangleShape filoSuperior_;
    sf::RectangleShape filoIzquierdo_;
    sf::RectangleShape marcaTecla_;
    sf::Text           texto_;
    sf::Text           tecla_;
    Accion             accion_;

    sf::Color colorBase_      = tema::kBoton;
    sf::Color colorResaltado_ = tema::kBotonHover;

    bool resaltado_  = false;
    bool habilitado_ = true;
    bool disponible_ = true;
    bool activo_     = false;

    sf::Clock relojHover_;
    float     desplazamientoHover_ = 0.f;
};

}
