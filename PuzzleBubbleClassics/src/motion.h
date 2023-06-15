#pragma once

#include "object_basics.h"
#include "sprite.h"


class DefaultMotionObject : public sf::Transformable, public virtual Updatable
{
private:
	sf::Vector2f _speed;
	sf::Vector2f _acceleration;
	float _rotationSpeed = 0;
	float _rotationAcceleration = 0;

public:
	DefaultMotionObject() = default;
	DefaultMotionObject(const DefaultMotionObject&) = default;
	DefaultMotionObject(DefaultMotionObject&&) noexcept = default;
	~DefaultMotionObject() = default;

	DefaultMotionObject& operator= (const DefaultMotionObject&) = default;
	DefaultMotionObject& operator= (DefaultMotionObject&&) = default;

public:
	inline const sf::Vector2f& getSpeed() const { return _speed; }
	inline void setSpeed(const sf::Vector2f& speed) { _speed = speed; }

	inline const sf::Vector2f& getAcceleration() const { return _acceleration; }
	inline void setAcceleration(const sf::Vector2f& acceleration) { _acceleration = acceleration; }

	constexpr float getRotationSpeed() const { return _rotationSpeed; }
	constexpr void setRotationSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed; }

	constexpr float getRotationAcceleration() const { return _rotationAcceleration; }
	constexpr void setRotationAcceleration(float rotationAcceleration) { _rotationAcceleration = rotationAcceleration; }

public:
	void update(const sf::Time& elapsedTime) override;
};


class SpriteObject;
class InternalSpriteMotionObject;
class SpriteMotionObject;
class AnimationObject;


class InternalSpriteObject : public virtual Renderable
{
public:
	friend SpriteObject;
	friend InternalSpriteMotionObject;
	friend SpriteMotionObject;
	friend AnimationObject;

private:
	Sprite _sprite;

public:
	InternalSpriteObject() = default;
	InternalSpriteObject(const InternalSpriteObject&) = default;
	InternalSpriteObject(InternalSpriteObject&&) noexcept = default;
	~InternalSpriteObject() = default;

	InternalSpriteObject& operator= (const InternalSpriteObject&) = default;
	InternalSpriteObject& operator= (InternalSpriteObject&&) = default;

public:
	inline InternalSpriteObject(const Sprite& sprite) : Renderable(), _sprite(sprite) {}
	inline InternalSpriteObject(Sprite&& sprite) : Renderable(), _sprite(std::move(sprite)) {}

	constexpr bool hasSprite() const { return _sprite == nullptr; }
	inline Sprite& getSprite() { return _sprite; }
	inline const Sprite& getSprite() const { return _sprite; }

public:
	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override;
};



class SpriteObject : public InternalSpriteObject
{
public:
	SpriteObject() = default;
	SpriteObject(const SpriteObject&) = default;
	SpriteObject(SpriteObject&&) noexcept = default;
	~SpriteObject() = default;

	SpriteObject& operator= (const SpriteObject&) = default;
	SpriteObject& operator= (SpriteObject&&) = default;

public:
	inline void setSprite(const Sprite& sprite) { _sprite = sprite, onSetSprite(_sprite); }
	inline void setSprite(Sprite&& sprite) { _sprite = std::move(sprite), onSetSprite(_sprite); }

protected:
	virtual void onSetSprite(Sprite& sprite);
};



class InternalSpriteMotionObject : public DefaultMotionObject, public InternalSpriteObject
{
public:
	InternalSpriteMotionObject() = default;
	InternalSpriteMotionObject(const InternalSpriteMotionObject&) = default;
	InternalSpriteMotionObject(InternalSpriteMotionObject&&) noexcept = default;
	~InternalSpriteMotionObject() = default;

	InternalSpriteMotionObject& operator= (const InternalSpriteMotionObject&) = default;
	InternalSpriteMotionObject& operator= (InternalSpriteMotionObject&&) = default;

public:
	inline InternalSpriteMotionObject(const Sprite& sprite) : DefaultMotionObject(), InternalSpriteObject(sprite) {}
	inline InternalSpriteMotionObject(Sprite&& sprite) : DefaultMotionObject(), InternalSpriteObject(sprite) {}

public:
	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override;
	void update(const sf::Time& elapsedTime) override;
};


class SpriteMotionObject : public InternalSpriteMotionObject
{
public:
	SpriteMotionObject() = default;
	SpriteMotionObject(const SpriteMotionObject&) = default;
	SpriteMotionObject(SpriteMotionObject&&) noexcept = default;
	~SpriteMotionObject() = default;

	SpriteMotionObject& operator= (const SpriteMotionObject&) = default;
	SpriteMotionObject& operator= (SpriteMotionObject&&) = default;

public:
	inline void setSprite(const Sprite& sprite) { _sprite = sprite, onSetSprite(_sprite); }
	inline void setSprite(Sprite&& sprite) { _sprite = std::move(sprite), onSetSprite(_sprite); }

protected:
	virtual void onSetSprite(Sprite& sprite);
};



class AnimationObject : public InternalSpriteMotionObject
{
public:
	inline AnimationObject() : InternalSpriteMotionObject(Sprite::makeAnimated()) {}
	AnimationObject(const AnimationObject&) = default;
	AnimationObject(AnimationObject&&) noexcept = default;
	~AnimationObject() = default;

	AnimationObject& operator= (const AnimationObject&) = default;
	AnimationObject& operator= (AnimationObject&&) = default;

public:
	inline void setSize(float width, float height) { _sprite.setSize(width, height); }
	inline void setSize(const sf::Vector2f& size) { _sprite.setSize(size); }

	inline float getWidth() const { return _sprite.getWidth(); }
	inline float getHeight() const { return _sprite.getHeight(); }
	inline sf::Vector2f getSize() const { return _sprite.getSize(); }

	inline float getUnscaledWidth() const { return _sprite.getUnscaledWidth(); }
	inline float getUnscaledHeight() const { return _sprite.getUnscaledHeight(); }
	inline sf::Vector2f getUnscaledSize() const { return _sprite.getUnscaledSize(); }
};
