#pragma once

#include "object_basics.h"

#include "utils/manager.h"
#include "utils/path.h"
#include "utils/fasttypes.h"
#include "utils/rng.h"
#include "utils/logger.h"

#include <SFML/Graphics.hpp>


class TextureManager : Manager<sf::Texture>
{
private:
	static TextureManager Root;

public:
	bool load(const Path& filepath, const std::string& tag, const sf::IntRect& dims);

public:
	inline TextureManager createChild() const { return TextureManager(const_cast<TextureManager*>(this)); }

	inline bool load(const Path& filepath, const std::string& tag) { return load(filepath, tag, {}); }
	inline bool load(const Path& filepath, const std::string& tag, int x, int y, int width, int height) { return load(filepath, tag, { x, y, width, height }); }

private:
	inline explicit TextureManager(TextureManager* parent) : Manager(parent) {}

public:
	static constexpr TextureManager& root() { return Root; }
};




class Sprite;
class StaticSprite;
class AnimatedSprite;
class RandomAnimatedSprite;


enum class SpriteType
{
	Static,
	Animated,
	RandomAnimated
};


class Sprite : public sf::Sprite, public Updatable
{
public:
	using Type = SpriteType;

	friend StaticSprite;
	friend AnimatedSprite;
	friend RandomAnimatedSprite;

private:
	Type _type;

private:
	inline explicit Sprite(Type type) : sf::Sprite(), _type(type) {}
	inline Sprite(Type type, const sf::Texture& texture) : sf::Sprite(texture), _type(type) {}
	inline Sprite(Type type, const sf::Texture& texture, const sf::IntRect& rectangle) : sf::Sprite(texture, rectangle), _type(type) {}

	Sprite(const Sprite&) = default;
	Sprite(Sprite&&) noexcept = default;

	Sprite& operator= (const Sprite&) = default;
	Sprite& operator= (Sprite&&) noexcept = default;

public:
	virtual ~Sprite() = default;

public:
	constexpr void update(const sf::Time& elapsedTime) override {}

	constexpr Type getType() const { return _type; }

	constexpr bool isStatic() const { return _type == Type::Static; }
	constexpr bool isAnimated() const { return _type == Type::Animated || _type == Type::RandomAnimated; }
	constexpr bool isRandomAnimated() const { return _type == Type::RandomAnimated; }

	inline int width() const { return int(getTextureRect().width * getScale().x); }
	inline int height() const { return int(getTextureRect().height * getScale().y); }

public:
	inline StaticSprite& asStatic();
	inline const StaticSprite& asStatic() const;

	inline AnimatedSprite& asAnimated();
	inline const AnimatedSprite& asAnimated() const;

	inline RandomAnimatedSprite& asRandom();
	inline const RandomAnimatedSprite& asRandom() const;

public:
	virtual std::unique_ptr<Sprite> makeUniqueCopy() = 0;
	virtual std::shared_ptr<Sprite> makeSharedCopy() = 0;

public:
	template <std::derived_from<Sprite> _Ty>
	static inline std::unique_ptr<Sprite> upcast(std::unique_ptr<_Ty>&& ptr);

	template <std::derived_from<Sprite> _Ty>
	static inline std::shared_ptr<Sprite> upcast(const std::shared_ptr<_Ty>& ptr);

	template <std::derived_from<Sprite> _Ty>
	static inline std::shared_ptr<Sprite> upcast(std::shared_ptr<_Ty>&& ptr);

public:
	static inline std::unique_ptr<StaticSprite> makeUniqueStatic();
	static inline std::unique_ptr<StaticSprite> makeUniqueStatic(const sf::Texture& texture);
	static inline std::unique_ptr<StaticSprite> makeUniqueStatic(const sf::Texture& texture, const sf::IntRect& rectangle);
	static inline std::shared_ptr<StaticSprite> makeSharedStatic();
	static inline std::shared_ptr<StaticSprite> makeSharedStatic(const sf::Texture& texture);
	static inline std::shared_ptr<StaticSprite> makeSharedStatic(const sf::Texture& texture, const sf::IntRect& rectangle);

	static inline std::unique_ptr<AnimatedSprite> makeUniqueAnimated();
	static inline std::unique_ptr<AnimatedSprite> makeUniqueAnimated(const sf::Texture& texture);
	static inline std::unique_ptr<AnimatedSprite> makeUniqueAnimated(const sf::Texture& texture, const sf::IntRect& rectangle);
	static inline std::shared_ptr<AnimatedSprite> makeSharedAnimated();
	static inline std::shared_ptr<AnimatedSprite> makeSharedAnimated(const sf::Texture& texture);
	static inline std::shared_ptr<AnimatedSprite> makeSharedAnimated(const sf::Texture& texture, const sf::IntRect& rectangle);

	static inline std::unique_ptr<RandomAnimatedSprite> makeUniqueRandom();
	static inline std::unique_ptr<RandomAnimatedSprite> makeUniqueRandom(const sf::Texture& texture);
	static inline std::unique_ptr<RandomAnimatedSprite> makeUniqueRandom(const sf::Texture& texture, const sf::IntRect& rectangle);
	static inline std::shared_ptr<RandomAnimatedSprite> makeSharedRandom();
	static inline std::shared_ptr<RandomAnimatedSprite> makeSharedRandom(const sf::Texture& texture);
	static inline std::shared_ptr<RandomAnimatedSprite> makeSharedRandom(const sf::Texture& texture, const sf::IntRect& rectangle);

	template <std::derived_from<Sprite> _Ty, std::derived_from<_Ty> _Ty2>
	static inline void safeSetSprite(std::unique_ptr<_Ty>& target, std::unique_ptr<_Ty2>&& src);

	template <std::derived_from<Sprite> _Ty, std::derived_from<_Ty> _Ty2>
	static inline void safeSetSprite(std::shared_ptr<_Ty>& target, std::shared_ptr<_Ty2>&& src);
};



class StaticSprite final : public Sprite
{
public:
	inline StaticSprite() : Sprite(Type::Static) {}
	inline explicit StaticSprite(const sf::Texture& texture) : Sprite(Type::Static, texture) {}
	inline StaticSprite(const sf::Texture& texture, const sf::IntRect& rectangle) : Sprite(Type::Static, texture, rectangle) {}

	StaticSprite(const StaticSprite&) = default;
	StaticSprite(StaticSprite&&) noexcept = default;

	StaticSprite& operator= (const StaticSprite&) = default;
	StaticSprite& operator= (StaticSprite&&) noexcept = default;

	virtual ~StaticSprite() = default;

public:
	inline std::unique_ptr<Sprite> makeUniqueCopy() override { return std::unique_ptr<Sprite>(new StaticSprite(*this)); }
	inline std::shared_ptr<Sprite> makeSharedCopy() override { return std::shared_ptr<Sprite>(new StaticSprite(*this)); }
};



class AnimatedSprite : public Sprite
{
private:
	sf::IntRect _firstFrame = { 0, 0, 1, 1 };
	std::size_t _framesCount = 1;
	float _speed = 1;
	bool _loop = false;
	bool _end = true;
	float _it = 0;

protected:
	inline explicit AnimatedSprite(Type type) : Sprite(type) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Texture& texture) : Sprite(type, texture) { updateTextureRect(); }
	inline explicit AnimatedSprite(Type type, const sf::Texture& texture, const sf::IntRect& rectangle) : Sprite(type, texture, rectangle), _firstFrame(rectangle) { updateTextureRect(); }

public:
	inline AnimatedSprite() : AnimatedSprite(Type::Animated) {}
	inline AnimatedSprite(const sf::Texture& texture) : AnimatedSprite(Type::Animated, texture) {}
	inline AnimatedSprite(const sf::Texture& texture, const sf::IntRect& rectangle) : AnimatedSprite(Type::Animated, texture, rectangle) {}

	AnimatedSprite(const AnimatedSprite&) = default;
	AnimatedSprite(AnimatedSprite&&) noexcept = default;

	AnimatedSprite& operator= (const AnimatedSprite&) = default;
	AnimatedSprite& operator= (AnimatedSprite&&) noexcept = default;

	virtual ~AnimatedSprite() = default;

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
	constexpr Int2d<int> getFrameSize() const { return { _firstFrame.width, _firstFrame.height }; }
	constexpr Int2d<int> getFirstFramePosition() const { return { _firstFrame.left, _firstFrame.top }; }

	inline void rewind() { _it = 0, _end = false, updateTextureRect(); }
	inline void fastForward() { _it = float(_framesCount), _end = true, updateTextureRect(); }

	inline std::unique_ptr<Sprite> makeUniqueCopy() override { return std::unique_ptr<Sprite>(new AnimatedSprite(*this)); }
	inline std::shared_ptr<Sprite> makeSharedCopy() override { return std::shared_ptr<Sprite>(new AnimatedSprite(*this)); }

public:
	void update(const sf::Time& elapsedTime) override;

private:
	constexpr bool hasMoreFrames() const { return _it >= 0 && std::size_t(_it) < _framesCount; }

	inline void updateTextureRect()
	{
		sf::Sprite::setTextureRect({
			_firstFrame.left + int(_it),
			_firstFrame.top,
			_firstFrame.width,
			_firstFrame.height
		});
	}
};



class RandomAnimatedSprite final : public AnimatedSprite
{
private:
	sf::Time _min = sf::Time::Zero;
	sf::Time _max = sf::seconds(10);
	sf::Time _current = sf::Time::Zero;
	RNG _rand;

public:
	inline RandomAnimatedSprite() : AnimatedSprite(Type::RandomAnimated) {}
	inline RandomAnimatedSprite(const sf::Texture& texture) : AnimatedSprite(Type::RandomAnimated, texture) {}
	inline RandomAnimatedSprite(const sf::Texture& texture, const sf::IntRect& rectangle) : AnimatedSprite(Type::RandomAnimated, texture, rectangle) {}

	RandomAnimatedSprite(const RandomAnimatedSprite&) = default;
	RandomAnimatedSprite(RandomAnimatedSprite&&) noexcept = default;

	RandomAnimatedSprite& operator= (const RandomAnimatedSprite&) = default;
	RandomAnimatedSprite& operator= (RandomAnimatedSprite&&) noexcept = default;

	virtual ~RandomAnimatedSprite() = default;

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

	inline std::unique_ptr<Sprite> makeUniqueCopy() override { return std::unique_ptr<Sprite>(new RandomAnimatedSprite(*this)); }
	inline std::shared_ptr<Sprite> makeSharedCopy() override { return std::shared_ptr<Sprite>(new RandomAnimatedSprite(*this)); }

public:
	void update(const sf::Time& elapsedTime) override;
};





inline StaticSprite& Sprite::asStatic() { return *reinterpret_cast<StaticSprite*>(this); }
inline const StaticSprite& Sprite::asStatic() const { return *reinterpret_cast<const StaticSprite*>(this); }

inline AnimatedSprite& Sprite::asAnimated() { return *reinterpret_cast<AnimatedSprite*>(this); }
inline const AnimatedSprite& Sprite::asAnimated() const { return *reinterpret_cast<const AnimatedSprite*>(this); }

inline RandomAnimatedSprite& Sprite::asRandom() { return *reinterpret_cast<RandomAnimatedSprite*>(this); }
inline const RandomAnimatedSprite& Sprite::asRandom() const { return *reinterpret_cast<const RandomAnimatedSprite*>(this); }


template <std::derived_from<Sprite> _Ty>
inline std::unique_ptr<Sprite> Sprite::upcast(std::unique_ptr<_Ty>&& ptr)
{
	return std::unique_ptr<Sprite>(std::move(ptr));
}

template <std::derived_from<Sprite> _Ty>
inline std::shared_ptr<Sprite> Sprite::upcast(const std::shared_ptr<_Ty>& ptr)
{
	return std::shared_ptr<Sprite>(std::move(ptr));
}

template <std::derived_from<Sprite> _Ty>
inline std::shared_ptr<Sprite> Sprite::upcast(std::shared_ptr<_Ty>&& ptr)
{
	return std::shared_ptr<Sprite>(ptr);
}


inline std::unique_ptr<StaticSprite> Sprite::makeUniqueStatic() { return std::make_unique<StaticSprite>(); }
inline std::unique_ptr<StaticSprite> Sprite::makeUniqueStatic(const sf::Texture& texture) { return std::make_unique<StaticSprite>(texture); }
inline std::unique_ptr<StaticSprite> Sprite::makeUniqueStatic(const sf::Texture& texture, const sf::IntRect& rectangle) { return std::make_unique<StaticSprite>(texture, rectangle); }
inline std::shared_ptr<StaticSprite> Sprite::makeSharedStatic() { return std::make_shared<StaticSprite>(); }
inline std::shared_ptr<StaticSprite> Sprite::makeSharedStatic(const sf::Texture& texture) { return std::make_shared<StaticSprite>(texture); }
inline std::shared_ptr<StaticSprite> Sprite::makeSharedStatic(const sf::Texture& texture, const sf::IntRect& rectangle) { return std::make_shared<StaticSprite>(texture, rectangle); }

inline std::unique_ptr<AnimatedSprite> Sprite::makeUniqueAnimated() { return std::make_unique<AnimatedSprite>(); }
inline std::unique_ptr<AnimatedSprite> Sprite::makeUniqueAnimated(const sf::Texture& texture) { return std::make_unique<AnimatedSprite>(texture); }
inline std::unique_ptr<AnimatedSprite> Sprite::makeUniqueAnimated(const sf::Texture& texture, const sf::IntRect& rectangle) { return std::make_unique<AnimatedSprite>(texture, rectangle); }
inline std::shared_ptr<AnimatedSprite> Sprite::makeSharedAnimated() { return std::make_shared<AnimatedSprite>(); }
inline std::shared_ptr<AnimatedSprite> Sprite::makeSharedAnimated(const sf::Texture& texture) { return std::make_shared<AnimatedSprite>(texture); }
inline std::shared_ptr<AnimatedSprite> Sprite::makeSharedAnimated(const sf::Texture& texture, const sf::IntRect& rectangle) { return std::make_shared<AnimatedSprite>(texture, rectangle); }

inline std::unique_ptr<RandomAnimatedSprite> Sprite::makeUniqueRandom() { return std::make_unique<RandomAnimatedSprite>(); }
inline std::unique_ptr<RandomAnimatedSprite> Sprite::makeUniqueRandom(const sf::Texture& texture) { return std::make_unique<RandomAnimatedSprite>(texture); }
inline std::unique_ptr<RandomAnimatedSprite> Sprite::makeUniqueRandom(const sf::Texture& texture, const sf::IntRect& rectangle) { return std::make_unique<RandomAnimatedSprite>(texture, rectangle); }
inline std::shared_ptr<RandomAnimatedSprite> Sprite::makeSharedRandom() { return std::make_shared<RandomAnimatedSprite>(); }
inline std::shared_ptr<RandomAnimatedSprite> Sprite::makeSharedRandom(const sf::Texture& texture) { return std::make_shared<RandomAnimatedSprite>(texture); }
inline std::shared_ptr<RandomAnimatedSprite> Sprite::makeSharedRandom(const sf::Texture& texture, const sf::IntRect& rectangle) { return std::make_shared<RandomAnimatedSprite>(texture, rectangle); }

template <std::derived_from<Sprite> _Ty, std::derived_from<_Ty> _Ty2>
inline void Sprite::safeSetSprite(std::unique_ptr<_Ty>& target, std::unique_ptr<_Ty2>&& src)
{
	if (src == nullptr)
	{
		logger::error("Attempt to set a nullptr Sprite");
		if constexpr (std::same_as<Sprite, _Ty>)
			target = Sprite::makeUniqueStatic();
		else
			target = std::make_unique<_Ty>();
	}
	target = std::move(src);
}

template <std::derived_from<Sprite> _Ty, std::derived_from<_Ty> _Ty2>
inline void Sprite::safeSetSprite(std::shared_ptr<_Ty>& target, std::shared_ptr<_Ty2>&& src)
{
	if (src == nullptr)
	{
		logger::error("Attempt to set a nullptr Sprite");
		if constexpr (std::same_as<Sprite, _Ty>)
			target = Sprite::makeSharedStatic();
		else
			target = std::make_shared<_Ty>();
	}
	target = std::move(src);
}
