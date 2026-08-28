#pragma once

#include "Boton.hpp"
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

/**
 * @brief Pantalla inicial: elegir especie, ponerle nombre y empezar.
 *
 * La vista previa usa la misma clase VistaMascota que la partida, asi que lo
 * que se ve aqui es exactamente lo que se vera jugando.
 */
class PantallaSeleccion : public Pantalla
{
public:
    PantallaSeleccion(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

    /// Entrega la mascota creada. Despues de llamarla, la pantalla ya no la tiene.
    std::unique_ptr<Mascota> tomarMascota();

private:
    void construirInterfaz();
    void elegirEspecie(TipoMascota tipo);
    void confirmar();
    void continuarPartida();
    void escribir(char32_t caracter);
    void centrar(sf::Text& texto, float y) const;

    const sf::Font&              fuente_;
    sf::Vector2f                 tamanoVentana_;

    /// Primero una entrada por especie, despues comenzar y continuar partida.
    std::vector<Boton>       botones_;
    std::unique_ptr<Mascota> mascota_;
    VistaMascota             vista_;

    std::string  nombre_;
    float        relojCursor_ = 0.f;
    bool         hayPartida_  = false;

    sf::RectangleShape panelPrevio_;
    sf::RectangleShape campoNombre_;
    sf::Text           titulo_;
    sf::Text           subtitulo_;
    sf::Text           etiquetaNombre_;
    sf::Text           textoNombre_;
    sf::Text           descripcion_;
    sf::Text           ayuda_;

    static constexpr std::size_t kMaxNombre = 14;
};

} // namespace vp
