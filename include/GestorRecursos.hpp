#pragma once

#include <SFML/Graphics.hpp>

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace vp {

/**
 * @brief Almacen generico de recursos (texturas, fuentes, sonidos).
 *
 * Es una plantilla, asi que sirve para cualquier tipo de SFML que tenga un
 * metodo loadFromFile(). Se guarda un solo ejemplar de cada recurso y se
 * reparten referencias: evita cargar la misma imagen veinte veces.
 *
 * Uso:
 *   GestorRecursos<sf::Texture> texturas;
 *   texturas.cargar("conejo", "assets/images/conejo_macho.png");
 *   sprite.setTexture(texturas.obtener("conejo"));
 */
template <typename Recurso>
class GestorRecursos
{
public:
    /// Carga el recurso y lo guarda bajo un identificador. false si fallo.
    bool cargar(const std::string& id, const std::string& ruta)
    {
        auto recurso = std::make_unique<Recurso>();

        // SFML 3 renombro Font::loadFromFile a Font::openFromFile, porque la
        // fuente no se carga entera en memoria: se abre y se lee bajo demanda.
        // El resto de recursos conserva loadFromFile.
        bool cargado = false;
        if constexpr (std::is_same_v<Recurso, sf::Font>)
            cargado = recurso->openFromFile(ruta);
        else
            cargado = recurso->loadFromFile(ruta);

        if (!cargado) return false;

        recursos_[id] = std::move(recurso);
        return true;
    }

    /// Devuelve el recurso. Lanza std::runtime_error si el id no existe.
    Recurso& obtener(const std::string& id)
    {
        auto it = recursos_.find(id);
        if (it == recursos_.end())
            throw std::runtime_error("Recurso no encontrado: " + id);
        return *it->second;
    }

    const Recurso& obtener(const std::string& id) const
    {
        auto it = recursos_.find(id);
        if (it == recursos_.end())
            throw std::runtime_error("Recurso no encontrado: " + id);
        return *it->second;
    }

    bool contiene(const std::string& id) const
    {
        return recursos_.find(id) != recursos_.end();
    }

    void liberar(const std::string& id) { recursos_.erase(id); }
    void liberarTodo()                  { recursos_.clear(); }
    std::size_t cantidad() const        { return recursos_.size(); }

private:
    std::map<std::string, std::unique_ptr<Recurso>> recursos_;
};

} // namespace vp
