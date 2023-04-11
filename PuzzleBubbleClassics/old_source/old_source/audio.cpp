#include "audio.h"
#include "resources.h"


SoundManager::SoundManager() : SimpleElementManager(globals::sounds()) {}
SoundManager::SoundManager(SoundManager& parent) : SimpleElementManager(parent) {}
SoundManager::SoundManager(int) : SimpleElementManager() {}

bool SoundManager::load(const String& filepath, const String& name)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(resource::sounds.pathOf(filepath).string()))
		return false;

	auto sound = create<sf::Sound>(name);
	sound->setBuffer(std::move(buffer));
	return true;
}




void Music::open(const String& filename)
{
	if (!isValid())
	{
		auto m = new sf::Music();
		if (!m->openFromFile(resource::music.pathOf(filename).string()))
			delete m;
		else
			_music.reset(m);
	}
}

void Music::close()
{
	_music.reset();
}

bool Music::isOpen() const { return isValid(); }

void Music::play()
{
	if (isValid())
		_music->play();
}

void Music::stop()
{
	if (isValid())
		_music->stop();
}

void Music::pause()
{
	if (isValid())
		_music->pause();
}
