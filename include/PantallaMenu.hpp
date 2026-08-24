#ifndef PANTALLA_MENU_HPP
#define PANTALLA_MENU_HPP

#include "Music.hpp"
#include "MusicButton.hpp"
#include "Pantalla.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace vp {

/// Opciones del menu principal, en el orden en que aparecen.
enum class OpcionMenu
{
    Jugar = 0,
    Opciones,
    Salir
};

/**
 * @brief Menu principal del juego.
 *
 * Mantiene la forma del menu del Proyecto3P: fondo a pantalla completa, titulo
 * inclinado arriba, opciones en vertical con la seleccionada en amarillo, e
 * icono de altavoz en la esquina para silenciar la musica. Se maneja con
 * teclado y con raton.
 *
 * La diferencia con aquel es que alli Mostrar() era un bucle que bloqueaba
 * hasta que el jugador elegia. Aqui el menu es una Pantalla mas: el bucle vive
 * en Juego y esta clase solo responde a eventos y pide la transicion. Asi las
 * pantallas se combinan sin anidar bucles unas dentro de otras.
 */
class PantallaMenu : public Pantalla
{
public:
    PantallaMenu(const sf::Font& fuente, sf::Vector2f tamanoVentana);

    void manejarEvento(const sf::Event& evento) override;
    void actualizar(float dt) override;
    void dibujar(sf::RenderTarget& objetivo) const override;

private:
    void construirFondo();
    void colocarOpciones();
    void elegir(std::size_t indice);
    void mover(int direccion);

    const sf::Font& fuente_;
    sf::Vector2f    tamanoVentana_;

    /// El fondo se compone una sola vez sobre una textura y luego se dibuja
    /// como un unico sprite, en vez de repetir cientos de sprites por fotograma.
    sf::RenderTexture         lienzoFondo_;
    std::optional<sf::Sprite> fondo_;

    sf::Text              titulo_;
    std::vector<sf::Text> textosMenu_;
    sf::Text              pie_;

    Music       musica_;
    MusicButton botonMusica_;

    std::size_t  seleccion_ = 0;
    sf::Vector2f raton_ { -1.f, -1.f };
    float        reloj_ = 0.f;
};

} // namespace vp

#endif // PANTALLA_MENU_HPP
