#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <SFML/Audio.hpp>

#include <string>

namespace vp {

/**
 * @brief Envoltorio sencillo sobre sf::Music para la musica de fondo.
 *
 * sf::Music no carga la pista entera en memoria: la lee del disco mientras
 * suena. Por eso el objeto tiene que seguir vivo todo el tiempo que dure la
 * reproduccion, y de ahi que las pantallas guarden su propia instancia.
 */
class Music
{
public:
    Music() = default;
    ~Music();

    Music(const Music&)            = delete;
    Music& operator=(const Music&) = delete;

    /// Abre el archivo. Devuelve false si no existe o el formato no se soporta.
    bool load(const std::string& archivo);

    void play();
    void pause();
    void stop();

    /// Repetir en bucle al llegar al final.
    void setLoop(bool repetir);

    /// Volumen de 0 a 100.
    void setVolume(float volumen);

    bool isPlaying() const;
    bool isLoaded() const { return cargada_; }

private:
    sf::Music musica_;
    bool      cargada_ = false;
};

} // namespace vp

#endif // MUSIC_HPP
