#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <SFML/Audio.hpp>

#include <string>

namespace vp {

class Music
{
public:
    Music() = default;
    ~Music();

    Music(const Music&)            = delete;
    Music& operator=(const Music&) = delete;

    bool load(const std::string& archivo);

    void play();
    void pause();
    void stop();

    void setLoop(bool repetir);

    void setVolume(float volumen);

    bool isPlaying() const;
    bool isLoaded() const { return cargada_; }

private:
    sf::Music musica_;
    bool      cargada_ = false;
};

}

#endif
