#pragma once

#include "Boton.hpp"
#include "Genero.hpp"
#include "Pantalla.hpp"
#include "VistaMascota.hpp"
#include "FabricaMascotas.hpp"
#include "Mascota.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace vp {

class PantallaSeleccion : public Pantalla
{
public:
    PantallaSeleccion(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

    std::unique_ptr<Mascota> tomarMascota();

private:
    void construirInterfaz();
    void elegirEspecie(TipoMascota tipo);
    void elegirGenero(Genero genero);

    void refrescarPrevia();

    void refrescarComenzar();

    void confirmar();
    void continuarPartida();
    void escribir(char32_t caracter);
    void centrar(sf::Text& texto, float y) const;

    std::string nombreParaMostrar() const;

    const sf::Font&              fuente_;
    sf::Vector2f                 tamanoVentana_;

    std::vector<Boton>       botones_;
    std::unique_ptr<Mascota> mascota_;
    VistaMascota             vista_;

    std::size_t inicioEspecies_ = 0;
    std::size_t inicioGeneros_  = 0;
    std::size_t indiceComenzar_ = 0;

    TipoMascota tipoElegido_   = TipoMascota::Conejo;
    Genero      generoElegido_ = Genero::Macho;

    std::string  nombre_;
    float        relojCursor_ = 0.f;
    bool         hayPartida_  = false;

    sf::RectangleShape panelPrevio_;
    sf::RectangleShape campoNombre_;
    sf::Text           titulo_;
    sf::Text           subtitulo_;
    sf::Text           etiquetaEspecie_;
    sf::Text           etiquetaGenero_;
    sf::Text           etiquetaNombre_;
    sf::Text           textoNombre_;
    sf::Text           descripcion_;
    sf::Text           ayuda_;

    static constexpr std::size_t kMaxNombre = 14;
};

}
