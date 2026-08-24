#pragma once

#include "Tema.hpp"

#include <SFML/Graphics.hpp>

#include <functional>
#include <string>
#include <utility>

namespace vp {

/**
 * @brief Boton rectangular con texto y una accion asociada.
 *
 * La accion se guarda como std::function, asi que el boton no necesita saber
 * nada de la mascota: quien lo crea le dice que debe hacer al pulsarlo. Eso
 * mantiene la interfaz desacoplada de la logica del juego.
 */
class Boton : public sf::Drawable
{
public:
    using Accion = std::function<void()>;

    Boton(const sf::Font& fuente, std::string etiqueta,
          sf::Vector2f posicion, sf::Vector2f tamano, Accion accion = nullptr);

    /// Actualiza el resaltado segun donde este el raton.
    void actualizar(sf::Vector2f posicionRaton);

    /// Devuelve true si el clic cayo dentro (y en ese caso ejecuta la accion).
    bool procesarClic(sf::Vector2f posicionClic);

    void establecerAccion(Accion accion)  { accion_ = std::move(accion); }
    void establecerHabilitado(bool habilitado);
    void establecerColores(sf::Color base, sf::Color resaltado);
    void establecerActivo(bool activo);        ///< marca el boton como "encendido"
    void establecerTamanoTexto(unsigned tamano);

    /// Etiqueta del atajo de teclado que se dibuja en la esquina del boton.
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
    bool activo_     = false;
};

} // namespace vp
