#include "sprite.h"

#include "data.h"


bool TextureManager::load(const Path& filepath, const std::string& tag, const sf::IntRect& dims)
{
	auto tex = emplace(tag);
	if (tex)
	{
		auto path = DataPool::instance().findFilePath(ResourceDirectoryType::Textures, filepath.string());
		if (!path.has_value() || !tex->loadFromFile(path->string(), dims))
		{
			destroy(tag);
			return false;
		}
		return true;
	}
	return false;
}



void AnimatedSprite::update(const sf::Time& elapsedTime)
{
	if (_end)
		return;

	_it += elapsedTime.asSeconds();
	if (hasMoreFrames())
	{
		updateTextureRect();
		return;
	}

	if (_loop)
	{
		while (!hasMoreFrames())
			_it = (_it < 0 ? _it + _framesCount : std::fmod(_it, float(_framesCount)));
		updateTextureRect();
		return;
	}
	_end = true;
}



void RandomAnimatedSprite::update(const sf::Time& elapsedTime)
{
	if (isWaiting())
	{
		_current -= elapsedTime;
		return;
	}

	setLoopEnabled(false);
	AnimatedSprite::update(elapsedTime);

	if (isFinished())
	{
		rewind();
		_current = (_max - _min) * _rand.randomFloat() + _min;
	}
}
