#include "Music.hpp"

namespace vp {

Music::~Music()
{
    stop();
}

bool Music::load(const std::string& archivo)
{

    cargada_ = musica_.openFromFile(archivo);
    return cargada_;
}

void Music::play()
{
    if (cargada_) musica_.play();
}

void Music::pause()
{
    if (cargada_) musica_.pause();
}

void Music::stop()
{
    if (cargada_) musica_.stop();
}

void Music::setLoop(bool repetir)
{

    if (cargada_) musica_.setLooping(repetir);
}

void Music::setVolume(float volumen)
{
    if (cargada_) musica_.setVolume(volumen);
}

bool Music::isPlaying() const
{
    return cargada_ && musica_.getStatus() == sf::SoundSource::Status::Playing;
}

}
