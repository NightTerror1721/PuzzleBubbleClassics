#pragma once

#include "object_basics.h"

#include "utils/manager.h"
#include "utils/path.h"
#include "utils/rng.h"
#include "utils/logger.h"
#include "utils/transform_utils.h"

#include <SFML/Graphics.hpp>


class TextureManager : public Manager<sf::Texture>
{
private:
	static TextureManager Instance;

public:
	bool load(const Path& filepath, const std::string& tag, const sf::IntRect& dims);

public:
	inline bool load(const Path& filepath, const std::string& tag) { return load(filepath, tag, {}); }
	inline bool load(const Path& filepath, const std::string& tag, int x, int y, int width, int height) { return load(filepath, tag, { x, y, width, height }); }

private:
	inline explicit TextureManager() : Manager(nullptr) {}

public:
	static constexpr TextureManager& instance() { return Instance; }
};




enum class SpriteType
{
	Invalid = 0,
	Static,
	Animated,
	RandomAnimated
};


class Sprite;
class StaticSprite;
class AnimatedSprite;
class RandomAnimatedSprite;

class AbstractSprite : public sf::Drawable, public sf::Transformable, public Updatable
{
public:
	friend ::Sprite;
	friend StaticSprite;
	friend AnimatedSprite;
	friend RandomAnimatedSprite;
	friend std::unique_ptr<AbstractSprite>;
	friend std::default_delete<AbstractSprite>;

public:
	using Type = SpriteType;

private:
	Type _type;
	sf::Vertex _vertices[4];
	ConstReference<sf::Texture> _texture = nullptr;
	sf::IntRect _textureRect = {};
	float _width = 1;
	float _height = 1;

private:
	inline explicit AbstractSprite(Type type) : sf::Transformable(), _type(type) {}
	inline AbstractSprite(Type type, const sf::Vector2f& size) : AbstractSprite(type) { setSize(size); }
	inline AbstractSprite(Type type, const sf::Texture& texture) : AbstractSprite(type) { setTexture(texture); }
	inline AbstractSprite(Type type, const sf::Texture& texture, const sf::IntRect& rectangle) : AbstractSprite(type)
	{
		setTexture(texture);
		setTextureRect(rectangle);
	}
	inline AbstractSprite(Type type, const sf::Vector2f& size, const sf::Texture& texture) : AbstractSprite(type)
	{
		setSize(size);
		setTexture(texture);
	}
	inline AbstractSprite(Type type, const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) : AbstractSprite(type)
	{
		setSize(size);
		setTexture(texture);
		setTextureRect(rectangle);
	}
	virtual ~AbstractSprite() = default;

private:
	AbstractSprite(const AbstractSprite&) = default;
	AbstractSprite(AbstractSprite&&) noexcept = default;

	AbstractSprite& operator= (const AbstractSprite&) = delete;
	AbstractSprite& operator= (AbstractSprite&&) noexcept = delete;

public:
	constexpr Type getType() const { return _type; }

	inline float getWidth() const { return _width * getScale().x; }
	inline float getHeight() const { return _height * getScale().y; }
	inline sf::Vector2f getSize() const { return { getWidth(), getHeight() }; }

	constexpr float getUnscaledWidth() const { return _width; }
	constexpr float getUnscaledHeight() const { return _height; }
	inline sf::Vector2f getUnscaledSize() const { return { getWidth(), getHeight() }; }

	inline void setWidth(float width) { setSize(width, _height); }
	inline void setHeight(float height) { setSize(_width, height); }
	inline void setSize(const sf::Vector2f& size) { setSize(size.x, size.y); }

	constexpr bool hasTexture() const { return _texture != nullptr; }
	constexpr ConstReference<sf::Texture> getTexture() const { return _texture; }

	constexpr const sf::IntRect& getTextureRect() const { return _textureRect; }

	inline void setColor(const sf::Color& color)
	{
		_vertices[0].color = color;
		_vertices[1].color = color;
		_vertices[2].color = color;
		_vertices[3].color = color;
	}
	inline const sf::Color& getColor() const { return _vertices[0].color; }

	inline sf::FloatRect getLocalBounds() const { return { 0.f, 0.f, _width, _height }; }
	inline sf::FloatRect getGlobalBounds() const { return getTransform().transformRect(getLocalBounds()); }

public:
	constexpr void update(const sf::Time& elapsedTime) override {}

public:
	void setTexture(const sf::Texture& texture, bool resetRect = false);

	void setTextureRect(const sf::IntRect& rectangle);

	void setSize(float width, float height);

private:
	virtual AbstractSprite* copy() const = 0;

	void draw(sf::RenderTarget& target, sf::RenderStates rs) const override;

	void updatePositions();
	void updateTexCoords();
};



class StaticSprite : public AbstractSprite
{
public:
	friend ::Sprite;
	friend std::unique_ptr<AbstractSprite>;
	friend std::unique_ptr<StaticSprite>;
	friend std::default_delete<AbstractSprite>;
	friend std::default_delete<StaticSprite>;

private:
	inline StaticSprite() : AbstractSprite(Type::Static) {}
	inline StaticSprite(const sf::Texture& texture) : AbstractSprite(Type::Static, texture) {}
	inline StaticSprite(const sf::Texture& texture, const sf::IntRect& rectangle) : AbstractSprite(Type::Static, texture, rectangle) {}
	inline StaticSprite(const sf::Vector2f& size) : AbstractSprite(Type::Static, size) {}
	inline StaticSprite(const sf::Vector2f& size, const sf::Texture& texture) : AbstractSprite(Type::Static, size, texture) {}
	inline StaticSprite(const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) : AbstractSprite(Type::Static, size, texture, rectangle) {}
	virtual ~StaticSprite() = default;

private:
	StaticSprite(const StaticSprite&) = default;
	StaticSprite(StaticSprite&&) noexcept = default;

	StaticSprite& operator= (const StaticSprite&) = delete;
	StaticSprite& operator= (StaticSprite&&) noexcept = delete;

private:
	inline AbstractSprite* copy() const { return new StaticSprite(*this); }
};




class AnimatedSprite : public AbstractSprite
{
public:
	friend ::Sprite;
	friend RandomAnimatedSprite;
	friend std::unique_ptr<AbstractSprite>;
	friend std::unique_ptr<AnimatedSprite>;
	friend std::default_delete<AbstractSprite>;
	friend std::default_delete<AnimatedSprite>;

private:
	sf::IntRect _firstFrame = { 0, 0, 1, 1 };
	std::size_t _framesCount = 1;
	float _speed = 1;
	bool _loop = false;
	bool _end = true;
	float _it = 0;

protected:
	inline explicit AnimatedSprite(Type type) : AbstractSprite(type) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Texture& texture) : AbstractSprite(type, texture) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Texture& texture, const sf::IntRect& rectangle) : AbstractSprite(type, texture, rectangle), _firstFrame(rectangle) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Vector2f& size) : AbstractSprite(type, size) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Vector2f& size, const sf::Texture& texture) : AbstractSprite(type, size, texture) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) :
		AbstractSprite(type, size, texture, rectangle), _firstFrame(rectangle) { updateTextureRect(); }

private:
	inline AnimatedSprite() : AnimatedSprite(Type::Animated) {}
	inline AnimatedSprite(const sf::Texture& texture) : AnimatedSprite(Type::Animated, texture) {}
	inline AnimatedSprite(const sf::Texture& texture, const sf::IntRect& rectangle) : AnimatedSprite(Type::Animated, texture, rectangle) {}
	inline AnimatedSprite(const sf::Vector2f& size) : AbstractSprite(Type::Animated, size) {}
	inline AnimatedSprite(const sf::Vector2f& size, const sf::Texture& texture) : AbstractSprite(Type::Animated, size, texture) {}
	inline AnimatedSprite(const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) : AbstractSprite(Type::Animated, size, texture, rectangle) {}
	virtual ~AnimatedSprite() = default;

private:
	AnimatedSprite(const AnimatedSprite&) = default;
	AnimatedSprite(AnimatedSprite&&) noexcept = default;

	AnimatedSprite& operator= (const AnimatedSprite&) = delete;
	AnimatedSprite& operator= (AnimatedSprite&&) noexcept = delete;

public:
	constexpr bool isFinished() const { return _end; }

	constexpr std::size_t getFramesCount() const { return _framesCount; }

	constexpr void setSpeed(float speed) { _speed = speed; }
	constexpr float getSpeed() const { return _speed; }

	constexpr void setLoopEnabled(bool enabled) { _loop = enabled; }
	constexpr bool isLoopEnabled() const { return _loop; }

	inline void setFirstFrameRect(const sf::IntRect& rect) { _firstFrame = rect, updateTextureRect(); }
	inline void setFrameSize(int width, int height) { _firstFrame.width = width, _firstFrame.height = height, updateTextureRect(); }
	inline void setFirstFramePosition(int x, int y) { _firstFrame.left = x, _firstFrame.top = y, updateTextureRect(); }
	inline void setFirstFrameRect(int x, int y, int width, int height)
	{
		_firstFrame.left = x, _firstFrame.top = y;
		_firstFrame.width = width, _firstFrame.height = height;
		updateTextureRect();
	}
	constexpr const sf::IntRect& getFirstFrameRect() const { return _firstFrame; }
	constexpr Dimensions2<int> getFrameSize() const { return { _firstFrame.width, _firstFrame.height }; }
	constexpr Dimensions2<int> getFirstFramePosition() const { return { _firstFrame.left, _firstFrame.top }; }

	inline void rewind() { _it = 0, _end = false, updateTextureRect(); }
	inline void fastForward() { _it = float(_framesCount), _end = true, updateTextureRect(); }

public:
	void update(const sf::Time& elapsedTime) override;

private:
	constexpr bool hasMoreFrames() const { return _it >= 0 && std::size_t(_it) < _framesCount; }

	inline void updateTextureRect()
	{
		setTextureRect({
			_firstFrame.left + int(_it),
			_firstFrame.top,
			_firstFrame.width,
			_firstFrame.height
			});
	}

private:
	inline AbstractSprite* copy() const { return new AnimatedSprite(*this); }
};




class RandomAnimatedSprite final : public AnimatedSprite
{
public:
	friend ::Sprite;
	friend std::unique_ptr<AbstractSprite>;
	friend std::unique_ptr<AnimatedSprite>;
	friend std::unique_ptr<RandomAnimatedSprite>;
	friend std::default_delete<AbstractSprite>;
	friend std::default_delete<AnimatedSprite>;
	friend std::default_delete<RandomAnimatedSprite>;

private:
	sf::Time _min = sf::Time::Zero;
	sf::Time _max = sf::seconds(10);
	sf::Time _current = sf::Time::Zero;
	RNG _rand;

private:
	inline RandomAnimatedSprite() : AnimatedSprite(Type::RandomAnimated) {}
	inline RandomAnimatedSprite(const sf::Texture& texture) : AnimatedSprite(Type::RandomAnimated, texture) {}
	inline RandomAnimatedSprite(const sf::Texture& texture, const sf::IntRect& rectangle) : AnimatedSprite(Type::RandomAnimated, texture, rectangle) {}
	inline RandomAnimatedSprite(const sf::Vector2f& size) : AnimatedSprite(Type::RandomAnimated, size) {}
	inline RandomAnimatedSprite(const sf::Vector2f& size, const sf::Texture& texture) : AnimatedSprite(Type::RandomAnimated, size, texture) {}
	inline RandomAnimatedSprite(const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) : AnimatedSprite(Type::RandomAnimated, size, texture, rectangle) {}
	virtual ~RandomAnimatedSprite() = default;

private:
	RandomAnimatedSprite(const RandomAnimatedSprite&) = default;
	RandomAnimatedSprite(RandomAnimatedSprite&&) noexcept = default;

	RandomAnimatedSprite& operator= (const RandomAnimatedSprite&) = delete;
	RandomAnimatedSprite& operator= (RandomAnimatedSprite&&) noexcept = delete;

	inline explicit RandomAnimatedSprite(const AnimatedSprite& sprite) : AnimatedSprite(sprite) {}
	inline explicit RandomAnimatedSprite(AnimatedSprite&& sprite) noexcept : AnimatedSprite(std::move(sprite)) {}

public:
	constexpr const sf::Time& getMinRandomDelay() const { return _min; }
	constexpr void setMinRandomDelay(const sf::Time& time) { _min = time; }

	constexpr const sf::Time& getMaxRandomDelay() const { return _max; }
	constexpr void setMaxRandomDelay(const sf::Time& time) { _max = time; }

	constexpr void setRandomDelay(const sf::Time& min, const sf::Time& max) { _min = min, _max = max; }

	constexpr const sf::Time& getRemainingTime() const { return _current; }

	inline bool isWaiting() const { return _current > sf::Time::Zero; }

public:
	void update(const sf::Time& elapsedTime) override;

private:
	inline AbstractSprite* copy() const { return new RandomAnimatedSprite(*this); }
};




class Sprite : public sf::Drawable, public Updatable
{
public:
	using Type = SpriteType;

private:
	std::unique_ptr<AbstractSprite> _sprite;

public:
	inline Sprite() : _sprite(new StaticSprite()) {}
	inline Sprite(std::unique_ptr<AbstractSprite>&& sprite) : _sprite(std::move(sprite)) {}
	~Sprite() = default;

public:
	inline Sprite(const Sprite& right) : _sprite(right.isValid() ? right._sprite->copy() : nullptr) {}
	inline Sprite(Sprite&& right) noexcept : _sprite(std::move(right._sprite)) {}

	inline Sprite& operator= (const Sprite& right) { return _sprite.reset(right.isValid() ? right._sprite->copy() : nullptr), *this; }
	inline Sprite& operator= (Sprite&& right) noexcept { return _sprite = std::move(right._sprite), *this; }

private:
	inline Sprite(AbstractSprite* sprite) : _sprite(sprite) {}

public:
	constexpr bool isValid() const { return _sprite != nullptr; }
	constexpr operator bool() const { return _sprite != nullptr; }
	constexpr bool operator! () const { return _sprite == nullptr; }
	constexpr bool operator== (decltype(nullptr)) const { return _sprite == nullptr; }

public:
	inline void setPosition(float x, float y) { _sprite->setPosition(x, y); }
	inline void setPosition(const sf::Vector2f& position) { _sprite->setPosition(position); }
	inline void setPosition(const Point2<float>& point) { _sprite->setPosition(point.x, point.y); }
	inline void setPosition(const Bounds2<float>& bounds) { _sprite->setPosition(bounds.x, bounds.y); }

	inline void setRotation(float angle) { _sprite->setRotation(angle); }

	inline void setScale(float factorX, float factorY) { _sprite->setScale(factorX, factorY); }
	inline void setScale(const sf::Vector2f& factors) { _sprite->setScale(factors); }

	inline void setOrigin(float x, float y) { _sprite->setOrigin(x, y); }
	inline void setOrigin(const sf::Vector2f& origin) { _sprite->setOrigin(origin); }

	inline const sf::Vector2f& getPosition() const { return _sprite->getPosition(); }
	inline const float getRotation() const { return _sprite->getRotation(); }
	inline const sf::Vector2f& getScale() const { return _sprite->getScale(); }
	inline const sf::Vector2f& getOrigin() const { return _sprite->getOrigin(); }

	inline void move(float offsetX, float offsetY) { _sprite->move(offsetX, offsetY); }
	inline void move(const sf::Vector2f& offset) { _sprite->move(offset); }

	inline void rotate(float angle) { _sprite->rotate(angle); }

	inline void scale(float factorX, float factorY) { _sprite->scale(factorX, factorY); }
	inline void scale(const sf::Vector2f& factors) { _sprite->scale(factors); }

	inline const sf::Transform& getTransform() const { return _sprite->getTransform(); }
	inline const sf::Transform& getInverseTransform() const { return _sprite->getInverseTransform(); }

	inline void setTexture(const sf::Texture& texture) { _sprite->setTexture(texture); }
	inline const sf::Texture& getTexture() const { return *_sprite->getTexture(); }
	inline bool hasTexture() const { return _sprite->getTexture() != nullptr; }

	inline void setTextureRect(const sf::IntRect& rectangle) { _sprite->setTextureRect(rectangle); }
	inline const sf::IntRect& getTextureRect() const { return _sprite->getTextureRect(); }

	inline void setColor(const sf::Color& color) { _sprite->setColor(color); }
	inline const sf::Color& getColor() const { return _sprite->getColor(); }

	inline sf::FloatRect getLocalBounds() const { return _sprite->getLocalBounds(); }
	inline sf::FloatRect getGlobalBounds() const { return _sprite->getGlobalBounds(); }

	constexpr Type getType() const { return _sprite == nullptr ? Type::Invalid : _sprite->getType(); }

	inline float getWidth() const { return _sprite->getWidth(); }
	inline float getHeight() const { return _sprite->getHeight(); }
	inline sf::Vector2f getSize() const { return _sprite->getSize(); }

	inline float getUnscaledWidth() const { return _sprite->getUnscaledWidth(); }
	inline float getUnscaledHeight() const { return _sprite->getUnscaledHeight(); }
	inline sf::Vector2f getUnscaledSize() const { return _sprite->getUnscaledSize(); }

	inline void setWidth(float width) { _sprite->setWidth(width); }
	inline void setHeight(float height) { _sprite->setHeight(height); }
	inline void setSize(float width, float height) { _sprite->setSize(width, height); }
	inline void setSize(const sf::Vector2f& size) { _sprite->setSize(size); }
	inline void setSize(const Dimensions2<float>& size) { _sprite->setSize(size.width, size.height); }
	inline void setSize(const Bounds2<float>& bounds) { _sprite->setSize(bounds.width, bounds.height); }

public:
	inline void update(const sf::Time& elapsedTime) override { _sprite->update(elapsedTime); }

public:
	StaticSprite& asStatic();
	AnimatedSprite& asAnimated();
	RandomAnimatedSprite& asRandomAnimated();

	inline const StaticSprite& asStatic() const { return const_cast<Sprite*>(this)->asStatic(); }
	inline const AnimatedSprite& asAnimated() const { return const_cast<Sprite*>(this)->asAnimated(); }
	inline const RandomAnimatedSprite& asRandomAnimated() const { return const_cast<Sprite*>(this)->asRandomAnimated(); }

protected:
	inline void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(*_sprite, states); }

public:
	inline static Sprite makeStatic() { return new StaticSprite(); }
	inline static Sprite makeStatic(const sf::Texture& texture) { return new StaticSprite(texture); }
	inline static Sprite makeStatic(const sf::Texture& texture, const sf::IntRect& rectangle) { return new StaticSprite(texture, rectangle); }
	inline static Sprite makeStatic(const sf::Vector2f& size) { return new StaticSprite(size); }
	inline static Sprite makeStatic(const sf::Vector2f& size, const sf::Texture& texture) { return new StaticSprite(size, texture); }
	inline static Sprite makeStatic(const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) { return new StaticSprite(size, texture, rectangle); }

	inline static Sprite makeAnimated() { return new AnimatedSprite(); }
	inline static Sprite makeAnimated(const sf::Texture& texture) { return new AnimatedSprite(texture); }
	inline static Sprite makeAnimated(const sf::Texture& texture, const sf::IntRect& rectangle) { return new AnimatedSprite(texture, rectangle); }
	inline static Sprite makeAnimated(const sf::Vector2f& size) { return new AnimatedSprite(size); }
	inline static Sprite makeAnimated(const sf::Vector2f& size, const sf::Texture& texture) { return new AnimatedSprite(size, texture); }
	inline static Sprite makeAnimated(const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) { return new AnimatedSprite(size, texture, rectangle); }

	inline static Sprite makeRandomAnimated() { return new RandomAnimatedSprite(); }
	inline static Sprite makeRandomAnimated(const sf::Texture& texture) { return new RandomAnimatedSprite(texture); }
	inline static Sprite makeRandomAnimated(const sf::Texture& texture, const sf::IntRect& rectangle) { return new RandomAnimatedSprite(texture, rectangle); }
	inline static Sprite makeRandomAnimated(const sf::Vector2f& size) { return new RandomAnimatedSprite(size); }
	inline static Sprite makeRandomAnimated(const sf::Vector2f& size, const sf::Texture& texture) { return new RandomAnimatedSprite(size, texture); }
	inline static Sprite makeRandomAnimated(const sf::Vector2f& size, const sf::Texture& texture, const sf::IntRect& rectangle) { return new RandomAnimatedSprite(size, texture, rectangle); }
};
