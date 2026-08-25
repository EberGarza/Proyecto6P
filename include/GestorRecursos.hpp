#pragma once

#include <SFML/Graphics.hpp>

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace vp {

template <typename Recurso>
class GestorRecursos
{
public:

    bool cargar(const std::string& id, const std::string& ruta)
    {
        auto recurso = std::make_unique<Recurso>();

        bool cargado = false;
        if constexpr (std::is_same_v<Recurso, sf::Font>)
            cargado = recurso->openFromFile(ruta);
        else
            cargado = recurso->loadFromFile(ruta);

        if (!cargado) return false;

        recursos_[id] = std::move(recurso);
        return true;
    }

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

}
