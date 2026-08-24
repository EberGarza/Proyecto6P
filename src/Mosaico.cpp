#include "Mosaico.hpp"

#include "Animacion.hpp"
#include "HojaSprites.hpp"
#include "TipoEstado.hpp"

#include <memory>
#include <optional>

namespace vp {

bool componerMosaico(sf::RenderTexture& lienzo,
                     sf::Vector2f tamano,
                     const std::vector<std::string>& hojas,
                     sf::Color base,
                     std::uint8_t alfa,
                     float paso,
                     float escala)
{
    const auto ancho = static_cast<unsigned>(tamano.x);
    const auto alto  = static_cast<unsigned>(tamano.y);
    if (!lienzo.resize({ ancho, alto })) return false;

    lienzo.clear(base);

    // HojaSprites no se puede copiar ni mover: cada Animacion guarda un puntero
    // a la textura de su hoja, y al mover el objeto ese puntero se quedaria
    // apuntando al original. Por eso van detras de unique_ptr y no por valor.
    std::vector<std::unique_ptr<HojaSprites>> cargadas;
    std::vector<sf::Sprite>                   sellos;

    for (const std::string& ruta : hojas)
    {
        auto hoja = std::make_unique<HojaSprites>();
        if (!hoja->cargar(ruta)) continue;

        const Animacion* idle = hoja->animacion(TipoEstado::Normal);
        if (!idle) continue;

        sf::Sprite sello(hoja->textura());
        idle->aplicarCuadroActual(sello);
        sello.setColor(sf::Color(255, 255, 255, alfa));
        sello.setScale({ escala, escala });

        sellos.push_back(sello);
        cargadas.push_back(std::move(hoja));   // mantiene viva la textura
    }

    if (!sellos.empty())
    {
        int fila = 0;
        for (float y = 20.f; y < tamano.y + paso; y += paso, ++fila)
        {
            // Filas alternas desplazadas, para que no se vea una rejilla.
            const float desfase = (fila % 2 == 0) ? 0.f : paso / 2.f;
            int columna = 0;

            for (float x = 20.f + desfase; x < tamano.x + paso; x += paso, ++columna)
            {
                // Tablero entre las hojas disponibles. Con una sola, se repite.
                sf::Sprite& sello = sellos[(fila + columna) % sellos.size()];

                sello.setRotation(sf::degrees(((fila + columna) % 5) * 7.f - 14.f));
                sello.setPosition({ x, y });
                lienzo.draw(sello);
            }
        }
    }

    lienzo.display();
    return true;
}

} // namespace vp
