#include "sprite.h"

#include "data.h"


TextureManager TextureManager::Instance;

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


void AbstractSprite::setTexture(const sf::Texture& texture, bool resetRect)
{
	if (resetRect || (!_texture && (_textureRect == sf::IntRect())))
		setTextureRect({ 0, 0, int(texture.getSize().x), int(texture.getSize().y) });

	_texture = std::addressof(texture);
}

void AbstractSprite::setTextureRect(const sf::IntRect& rectangle)
{
	if (rectangle != _textureRect)
	{
		_textureRect = rectangle;
		updateTexCoords();
	}
}

void AbstractSprite::setSize(float width, float height)
{
	if ((width != _width && width != 0) || (height != _height && height != 0))
	{
		width = std::abs(width);
		height = std::abs(height);

		_width = width;
		_height = height;
		updatePositions();
	}
}

void AbstractSprite::draw(sf::RenderTarget& target, sf::RenderStates rs) const
{
	if (_texture != nullptr)
	{
		rs.transform *= getTransform();
		rs.texture = &_texture;
		target.draw(_vertices, 4, sf::TriangleStrip, rs);
	}
}

void AbstractSprite::updatePositions()
{
	_vertices[0].position = { 0, 0 };
	_vertices[1].position = { 0, _height };
	_vertices[2].position = { _width, 0 };
	_vertices[3].position = { _width, _height };
}

void AbstractSprite::updateTexCoords()
{
	float left = float(_textureRect.left);
	float right = left + _textureRect.width;
	float top = float(_textureRect.top);
	float bottom = top + _textureRect.height;

	_vertices[0].texCoords = { left, top };
	_vertices[1].texCoords = { left, bottom };
	_vertices[2].texCoords = { right, top };
	_vertices[3].texCoords = { right, bottom };
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



StaticSprite& Sprite::asStatic()
{
	if (getType() != Type::Static)
		logger::error("Attempt to non Static sprite as Static.");
	return reinterpret_cast<StaticSprite&>(*_sprite);
}

AnimatedSprite& Sprite::asAnimated()
{
	if (getType() != Type::Animated)
		logger::error("Attempt to non Animated sprite as Animated.");
	return reinterpret_cast<AnimatedSprite&>(*_sprite);
}

RandomAnimatedSprite& Sprite::asRandomAnimated()
{
	if (getType() != Type::RandomAnimated)
		logger::error("Attempt to non RandomAnimated sprite as RandomAnimated.");
	return reinterpret_cast<RandomAnimatedSprite&>(*_sprite);
}
