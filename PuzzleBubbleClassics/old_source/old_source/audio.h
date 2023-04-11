#pragma once

#include "common.h"
#include "globals.h"

#include "utils/SimpleElementManager.h"

#include <SFML/Audio.hpp>


class SoundManager : SimpleElementManager<sf::Sound>
{
public:
	SoundManager();
	SoundManager(SoundManager& parent);

	bool load(const String& filepath, const String& name);

private:
	explicit SoundManager(int);

public:
	friend GlobalsManager;
};




class Music
{
private:
	std::unique_ptr<sf::Music> _music;

public:
	Music() = default;
	Music(const Music&) = delete;
	Music(Music&&) noexcept = default;
	~Music() = default;

	Music& operator= (const Music&) = delete;
	Music& operator= (Music&&) noexcept = default;

public:
	void open(const String& filename);
	void close();
	bool isOpen() const;

	void play();
	void stop();
	void pause();

public:
	inline bool isValid() const { return static_cast<bool>(_music); }

	inline operator bool() const { return static_cast<bool>(_music); }
	inline bool operator! () const { return !_music; }
};
