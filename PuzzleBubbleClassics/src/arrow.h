#pragma once

#include "bubble_gen.h"


class Scenario;
class Arrow;


class ArrowTextures : public SizedTransformable
{
private:
	static constexpr Bounds2<float> BoundsBase = { .x = -112.5f, .y = 94.5f, .width = 225.f, .height = 105.f };
	static constexpr Bounds2<float> GearBase[3] = {
		{ .x = -67.5f, .y = -37.5f },
		{ .x = 57.f, .y = -24.f },
		{ .x = -31.5f, .y = -31.5f }
	};
	static constexpr Point2<float> GearDelta[3] = {
		{ .x = 84.f, .y = 52.5f },
		{ .x = 84.f, .y = 52.5f },
		{ .x = 202.5f, .y = 90.f }
	};
	static constexpr Point2<float> LeverDelta = { .x = 39.f, .y = 64.5f };
	static constexpr Dimensions2<float> LeverSize = { .width = 36.f, .height = 36.f };
	static constexpr float Gear3Offset = 18;

private:
	ConstReference<Arrow> _arrow;
	Sprite _texture;
	Sprite _base;
	Sprite _base2;
	Sprite _lever;
	std::array<Sprite, 4> _gears;

public:
	ArrowTextures() = default;
	ArrowTextures(const ArrowTextures&) = delete;
	ArrowTextures(ArrowTextures&&) noexcept = default;
	virtual ~ArrowTextures() = default;

	ArrowTextures& operator= (const ArrowTextures&) = delete;
	ArrowTextures& operator= (ArrowTextures&&) noexcept = delete;

public:
	inline sf::FloatRect getLocalBounds() const { return { getPosition().x, getPosition().y, getWidth(), getHeight() }; }

public:
	void init(const Arrow& arrow);

	void drawBase(sf::RenderTarget& canvas, sf::RenderStates rs);
	void drawArrow(sf::RenderTarget& canvas, sf::RenderStates rs);

private:
	static Sprite loadSprite(std::string_view textureKey, std::string_view textureFilename);

	inline float pbx(float point) const { return (getWidth() * point) / _base.getWidth(); }
	inline float pby(float point) const { return (getHeight() * point) / _base.getHeight(); }

	inline void situateGear(std::size_t gearIdx)
	{
		_gears[gearIdx].setPosition(
			GearBase[gearIdx].x + pbx(GearDelta[0].x),
			GearBase[gearIdx].y + pby(GearDelta[0].y)
		);
		_gears[gearIdx].setSize(
			GearBase[gearIdx].width,
			GearBase[gearIdx].height
		);
	}
};


enum class ArrowMoveState
{
	None = 0,
	ArrowLeft = 1,
	ArrowRight = 2,
	ArrowUp = 3,
	Fire = 4,
	Slow = 5,
	Change = 6
};


class Arrow : public GameObject, public SizedTransformable
{
private:
	static constexpr Bounds2<float> BoundsOffset = { .x = -16.5f, .y = -97.5f + 21.f, .width = 33.f, .height = 180.f };

	//static constexpr float MaxSpeed = sf::radians(120.f / 180.f);

private:
	Reference<Scenario> _scenario;
	ArrowTextures _texturesBase;
	PlayerId _playerId = PlayerId::Single;
	std::shared_ptr<BubbleGenerator> _bgen;
//	std::shared_ptr<sf::Sound> _fireSound; TODO
	bool _enabledSwap = false;

	ArrowMoveState _moveState = ArrowMoveState::None;
	RequestFlag _swap = RequestFlag::Ready;
	bool _slowly = false;
	bool _doFire = false;
	bool _lockFire = false;
	bool _lock = false;
	float _angle = 0;
	float _moveSoundOffset = 0;
	float _lastPlayMoveSoundOffset = 0;
	std::shared_ptr<Bubble> _currentBubble;
	std::shared_ptr<Bubble> _nextBubble;

public:
	Arrow() = default;
	Arrow(const Arrow&) = delete;
	Arrow(Arrow&&) noexcept = default;
	virtual ~Arrow() = default;

	Arrow& operator= (const Arrow&) = delete;
	Arrow& operator= (Arrow&&) noexcept = delete;

public:
	inline sf::FloatRect getLocalBounds() const { return { getPosition().x, getPosition().y, getWidth(), getHeight() }; }

	inline void startBubbles() { _nextBubble = _bgen->makeBubble(true), makeNext(); }

	inline void unlockFire() { if (_lockFire) _lockFire = false, makeNext(); }

	constexpr bool isFireLocked() const { return _lockFire; }
	constexpr bool isLocked() const { return _lock; }

	inline void unlockArrow() { _lock = false, _lockFire = false; }

	inline sf::FloatRect getBaseLocalBounds() const { return _texturesBase.getLocalBounds(); }

	inline bool isBubbleFired() const { return _currentBubble == nullptr; }

public:
	void init(Scenario& scenario);

	void fireBubble();

public:
	inline void render(sf::RenderTarget& canvas, sf::RenderStates rs) override
	{
		rs.transform *= getTransform();
		_texturesBase.drawBase(canvas, rs);
		_texturesBase.drawArrow(canvas, rs);
	}

	void update(const sf::Time& elapsedTime) override;

private:
	void makeNext();

	void swapBubble();

private:

};
