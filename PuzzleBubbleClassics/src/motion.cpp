#include "motion.h"


void DefaultMotionObject::update(const sf::Time& elapsedTime)
{
	const float delta = elapsedTime.asSeconds();
	setPosition(getPosition() + (_speed * delta));
	_speed += _acceleration * delta;
	setRotation(getRotation() + (_rotationSpeed * delta));
	_rotationSpeed += _rotationAcceleration * delta;
}




void InternalSpriteObject::render(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	if (_sprite == nullptr)
		return;

	canvas.draw(_sprite, rs);
}


void InternalSpriteMotionObject::render(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	if (_sprite == nullptr)
		return;

	rs.transform *= getTransform();
	canvas.draw(_sprite, rs);
}


void InternalSpriteMotionObject::update(const sf::Time& elapsedTime)
{
	DefaultMotionObject::update(elapsedTime);
	if (_sprite != nullptr)
		_sprite.update(elapsedTime);
}


void SpriteMotionObject::onSetSprite(Sprite& sprite)
{
	//sprite.setPosition({});
	sprite.setRotation(0);
	sprite.setScale({ 1, 1 });
}
