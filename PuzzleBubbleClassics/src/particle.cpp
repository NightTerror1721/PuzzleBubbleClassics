#include "particle.h"


void Particle::update(const sf::Time& elapsedTime)
{
	if (isDead())
		return;

	_timeToLeave -= elapsedTime;
	setAlpha(_alpha + (_alphaSpeed * elapsedTime.asSeconds()));
	DefaultMotionObject::update(elapsedTime);
}


void SpriteParticle::render(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	if (isDead())
		return;

	rs.transform *= getTransform();
	rs.blendMode = sf::BlendAlpha;
	SpriteObject::render(canvas, rs);
}
