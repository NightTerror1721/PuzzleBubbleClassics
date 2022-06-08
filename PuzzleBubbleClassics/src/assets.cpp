#include "assets.h"
#include "common.h"
#include "resources.h"

TextureManager::TextureManager(int) : SimpleElementManager() {}
TextureManager::TextureManager() : SimpleElementManager(globals::textures()) {}
TextureManager::TextureManager(TextureManager& parent) : SimpleElementManager(parent) {}

bool TextureManager::load(const Path& filepath, const String& tag)
{
	auto tex = create(tag);
	if (tex)
	{
		if (!tex->loadFromFile(resource::textures.pathOf(filepath).string()))
		{
			destroy(tag);
			return false;
		}
		return true;
	}
	return false;
}

bool TextureManager::load(const Path& filepath, const String& tag, const sf::IntRect& dims)
{
	auto tex = create(tag);
	if (tex)
	{
		if (!tex->loadFromFile(resource::textures.pathOf(filepath).string(), dims))
		{
			destroy(tag);
			return false;
		}
		return true;
	}
	return false;
}

bool TextureManager::load(const Path& filepath, const String& tag, int x, int y, int width, int height)
{
	auto tex = create(tag);
	if (tex)
	{
		if (!tex->loadFromFile(resource::textures.pathOf(filepath).string(), { x, y, width, height }))
		{
			destroy(tag);
			return false;
		}
		return true;
	}
	return false;
}






AnimatedSprite::AnimatedSprite() :
	Sprite{}
{}
AnimatedSprite::~AnimatedSprite() {}

void AnimatedSprite::setFrameDimensions(UInt32 x, UInt32 y, UInt32 w, UInt32 h)
{
	_x = x;
	_y = y;
	_w = w;
	_h = h;
}
void AnimatedSprite::setFrameCount(UInt32 frames_count) { _frames = frames_count; }

void AnimatedSprite::setStaticMode() { _mode = Mode::Static; }
void AnimatedSprite::setSequenceMode() { _mode = Mode::Sequence; }
void AnimatedSprite::setLoopMode() { _mode = Mode::Loop; }
void AnimatedSprite::setRandomMode(float min, float max)
{
	_mode = Mode::Random;
	_min = std::min(min, max);
	_max = std::max(min, max);

	_rand = std::minstd_rand{ static_cast<unsigned int>(utils::systemTime()) };
	generateCurrent();
}

bool AnimatedSprite::isStaticMode() const { return _mode == Mode::Static; }
bool AnimatedSprite::isSequenceMode() const { return _mode == Mode::Sequence; }
bool AnimatedSprite::isLoopMode() const { return _mode == Mode::Loop; }
bool AnimatedSprite::isRandomMode() const { return _mode == Mode::Random; }

bool AnimatedSprite::hasEnded() const { return _end; }

void AnimatedSprite::start() { _end = false; }
void AnimatedSprite::stop() { _end = true; }

void AnimatedSprite::setCurrentFrame(UInt32 frame) { _it = static_cast<float>(frame); }
void AnimatedSprite::setExactCurrentFrame(float frame) { _it = frame; }

UInt32 AnimatedSprite::getCurrentFrame() const { return static_cast<UInt32>(_it); }
float AnimatedSprite::getExactCurrentFrame() const { return _it; }

void AnimatedSprite::rewind() { _it = 0.f; }
void AnimatedSprite::fastForward() { _it = static_cast<float>(_frames); }

UInt32 AnimatedSprite::getFrameX() const { return _x; }
UInt32 AnimatedSprite::getFrameY() const { return _y; }
UInt32 AnimatedSprite::getFrameWidth() const { return _w; }
UInt32 AnimatedSprite::getFrameHeight() const { return _h; }
UInt32 AnimatedSprite::getFrameCount() const { return _frames; }

#define try_move_it(delta) if(this->_end) { break; } this->_it += (delta).asSeconds() * this->_speed
void AnimatedSprite::update(const sf::Time& delta)
{
	switch (_mode)
	{
	case Mode::Sequence:
		try_move_it(delta);
		if (state())
			_end = true;
		break;

	case Mode::Loop:
		try_move_it(delta);
		updateIterator();
		break;

	case Mode::Random:
		try_move_it(delta);
		if (_current > 0)
			_current -= delta.asSeconds();
		else
		{
			if (state())
			{
				updateIterator();
				generateCurrent();
			}
		}
	}
}
#undef try_move_it

void AnimatedSprite::render(sf::RenderTarget& canvas)
{
	if (_mode == Mode::Static || !_end)
		canvas.draw(*this);
}

int AnimatedSprite::state() const { return _it >= static_cast<float>(_frames) ? 1 : _it < 0 ? -1 : 0; }

void AnimatedSprite::generateCurrent()
{
	const float maxval = _max - _min;
	float percentage = (_rand() / static_cast<float>(_rand.max()));
	_current = (maxval * percentage) + _min;
}

void AnimatedSprite::updateIterator()
{
	int current_state;
	while (current_state = state())
	{
		if (current_state > 0)
			_it -= static_cast<float>(_frames);
		else _it += static_cast<float>(_frames);
	}

	if (_it != _oldIt)
	{
		int it = static_cast<int>(_it);
		setTextureRect({
			static_cast<int>(_x + it),
			static_cast<int>(_y + it),
			static_cast<int>(_w),
			static_cast<int>(_h)
			});
	}
	_oldIt = _it;
}
