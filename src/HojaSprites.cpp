#include "HojaSprites.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

namespace vp {

std::string HojaSprites::carpetaDe(const std::string& ruta)
{
    const std::size_t corte = ruta.find_last_of("/\\");
    return (corte == std::string::npos) ? std::string() : ruta.substr(0, corte + 1);
}

void HojaSprites::recortarFondo(sf::Image& imagen, sf::Color fondo, int tolerancia)
{
    const sf::Vector2u tamano = imagen.getSize();
    const int umbral = tolerancia * tolerancia;

    for (unsigned y = 0; y < tamano.y; ++y)
    {
        for (unsigned x = 0; x < tamano.x; ++x)
        {
            const sf::Color pixel = imagen.getPixel({ x, y });

            const int dr = static_cast<int>(pixel.r) - fondo.r;
            const int dg = static_cast<int>(pixel.g) - fondo.g;
            const int db = static_cast<int>(pixel.b) - fondo.b;

            if (dr * dr + dg * dg + db * db <= umbral)
                imagen.setPixel({ x, y }, sf::Color(pixel.r, pixel.g, pixel.b, 0));
        }
    }
}

bool HojaSprites::cargar(const std::string& rutaConfig)
{
    animaciones_.clear();
    acciones_.clear();
    tintes_.clear();
    valida_ = false;
    error_.clear();

    std::ifstream archivo(rutaConfig);
    if (!archivo)
    {
        error_ = "No se pudo abrir " + rutaConfig;
        return false;
    }

    std::string rutaImagen;
    sf::Color   fondo(255, 255, 255);
    int         tolerancia = 0;

    bool                enConstruccion = false;
    TipoEstado          estadoActual   = TipoEstado::Normal;
    std::string         accionActual;
    std::vector<Cuadro> cuadros;
    float               duracion = 0.15f;
    bool                enBucle  = true;

    const auto cerrarAnimacion = [&]()
    {
        if (enConstruccion && !cuadros.empty())
        {
            Animacion animacion(textura_, cuadros, duracion, enBucle);

            if (accionActual.empty()) animaciones_.emplace(estadoActual, std::move(animacion));
            else                      acciones_.emplace(accionActual, std::move(animacion));
        }
        cuadros.clear();
        accionActual.clear();
        enConstruccion = false;
    };

    std::string linea;

    while (std::getline(archivo, linea))
    {

        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        const std::size_t comentario = linea.find('#');
        if (comentario != std::string::npos) linea = linea.substr(0, comentario);

        std::istringstream campos(linea);
        std::string clave;
        if (!(campos >> clave)) continue;

        if (clave == "imagen")
        {

            std::string resto;
            std::getline(campos, resto);

            const std::size_t inicio = resto.find_first_not_of(" \t");
            const std::size_t fin    = resto.find_last_not_of(" \t");
            if (inicio != std::string::npos)
                rutaImagen = resto.substr(inicio, fin - inicio + 1);
        }
        else if (clave == "nombre")
        {
            campos >> nombre_;
        }
        else if (clave == "fondo")
        {
            int r = 0, g = 0, b = 0;
            campos >> r >> g >> b;
            fondo = sf::Color(static_cast<std::uint8_t>(r),
                              static_cast<std::uint8_t>(g),
                              static_cast<std::uint8_t>(b));
        }
        else if (clave == "tolerancia")
        {
            campos >> tolerancia;
        }
        else if (clave == "escala")
        {
            campos >> escala_;
        }
        else if (clave == "animacion")
        {
            cerrarAnimacion();

            std::string nombreEstadoLeido;
            campos >> nombreEstadoLeido >> duracion;

            int bucle = 1;
            if (!(campos >> bucle)) bucle = 1;
            enBucle = (bucle != 0);

            enConstruccion = true;

            if (!estadoDesdeNombre(nombreEstadoLeido, estadoActual))
                accionActual = nombreEstadoLeido;
        }
        else if (clave == "tinte")
        {
            int r = 255, g = 255, b = 255;
            campos >> r >> g >> b;

            TipoEstado destino = estadoActual;
            std::string nombreEstadoLeido;

            if (!enConstruccion && campos >> nombreEstadoLeido)
                estadoDesdeNombre(nombreEstadoLeido, destino);

            tintes_[destino] = sf::Color(static_cast<std::uint8_t>(r),
                                         static_cast<std::uint8_t>(g),
                                         static_cast<std::uint8_t>(b));
        }
        else if (clave == "cuadro")
        {
            if (!enConstruccion) continue;

            int x = 0, y = 0, w = 0, h = 0;
            if (!(campos >> x >> y >> w >> h)) continue;

            Cuadro cuadro;
            cuadro.recorte = sf::IntRect({ x, y }, { w, h });

            float ox = 0.f, oy = 0.f;
            if (campos >> ox >> oy) cuadro.origen = { ox, oy };
            else                    cuadro.origen = { w * 0.5f, h * 0.5f };

            cuadros.push_back(cuadro);
        }
    }

    if (rutaImagen.empty())
    {
        error_ = "El archivo " + rutaConfig + " no indica ninguna imagen";
        return false;
    }

    sf::Image imagen;
    const std::string rutaCompleta = carpetaDe(rutaConfig) + rutaImagen;
    if (!imagen.loadFromFile(rutaCompleta))
    {
        error_ = "No se pudo cargar la imagen " + rutaCompleta;
        return false;
    }

    if (tolerancia > 0) recortarFondo(imagen, fondo, tolerancia);

    if (!textura_.loadFromImage(imagen))
    {
        error_ = "No se pudo crear la textura de " + rutaCompleta;
        return false;
    }
    textura_.setSmooth(false);

    cerrarAnimacion();

    valida_ = !animaciones_.empty();
    if (!valida_) error_ = "El archivo " + rutaConfig + " no define ninguna animacion";

    return valida_;
}

const Animacion* HojaSprites::animacion(TipoEstado tipo) const
{
    auto it = animaciones_.find(tipo);
    return (it == animaciones_.end()) ? nullptr : &it->second;
}

const Animacion* HojaSprites::accion(const std::string& nombre) const
{
    auto it = acciones_.find(nombre);
    return (it == acciones_.end()) ? nullptr : &it->second;
}

std::vector<std::string> HojaSprites::accionesDisponibles() const
{
    std::vector<std::string> nombres;
    nombres.reserve(acciones_.size());

    for (const auto& [nombre, animacion] : acciones_)
        nombres.push_back(nombre);

    return nombres;
}

sf::Color HojaSprites::tinte(TipoEstado tipo) const
{
    auto it = tintes_.find(tipo);
    return (it == tintes_.end()) ? sf::Color::White : it->second;
}

}
