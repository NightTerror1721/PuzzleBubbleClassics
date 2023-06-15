#pragma once

#include "motion.h"
#include "utils/math.h"


class Particle : public DefaultMotionObject, public virtual Renderable
{
private:
	float _alpha = 1;
	float _alphaSpeed = 0;
	sf::Time _timeToLeave = sf::Time::Zero;

public:
	Particle() = default;
	Particle(const Particle&) = delete;
	Particle(Particle&&) noexcept = default;
	~Particle() = default;

	Particle& operator= (const Particle&) = delete;
	Particle& operator= (Particle&&) noexcept = default;

public:
	constexpr float getAlpha() const { return _alpha; }
	constexpr float getAlphaSpeed() const { return _alphaSpeed; }
	inline const sf::Time& getTimeToLeave() const { return _timeToLeave; }
	inline bool isAlive() const { return _timeToLeave > sf::Time::Zero; }
	inline bool isDead() const { return _timeToLeave <= sf::Time::Zero; }

	inline void setAlpha(float alpha) { _alpha = utils::clamp(alpha, 0, 1), onSetAlpha(_alpha); }
	constexpr void setAlphaSpeed(float alphaSpeed) { _alphaSpeed = alphaSpeed; }
	inline void setTimeToLeave(const sf::Time& timeToLeave) { _timeToLeave = timeToLeave; }

public:
	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override = 0;
	void update(const sf::Time& elapsedTime) override;

protected:
	virtual constexpr void onSetAlpha(float alpha) {};
};


class SpriteParticle : public Particle, public SpriteObject
{
public:
	SpriteParticle() = default;
	SpriteParticle(const SpriteParticle&) = delete;
	SpriteParticle(SpriteParticle&&) noexcept = default;
	~SpriteParticle() = default;

	SpriteParticle& operator= (const SpriteParticle&) = delete;
	SpriteParticle& operator= (SpriteParticle&&) noexcept = default;

public:
	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override;

protected:
	void onSetAlpha(float alpha) override { getSprite().setColor({ 255, 255, 255, Uint8(alpha * 255) }); }
};
