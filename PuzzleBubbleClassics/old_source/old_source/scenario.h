#pragma once

#include "level.h"
#include "bubbles.h"
#include "bubble_board.h"


enum class ScenarioState
{
	InGame,
	Paused,
	Winning,
	Losing,
	Won,
	Lost
};

namespace scenarios
{
	constexpr bool isFinalState(ScenarioState state)
	{
		using enum ScenarioState;
		return state == Won || state == Lost;
	}

	constexpr bool isRunningState(ScenarioState state) { return !isFinalState(state); }

	constexpr bool isActiveRunningState(ScenarioState state) { return state == ScenarioState::InGame; }

	constexpr bool isPasiveRunningState(ScenarioState state)
	{
		using enum ScenarioState;
		return state == Paused || state == Winning || state == Losing;
	}

	constexpr bool isFinalizingState(ScenarioState state)
	{
		using enum ScenarioState;
		return state == Winning || state == Losing;
	}
}







class Particle : public sf::Transformable
{
private:
	sf::Vector2f _speed;
	sf::Vector2f _acceleration;
	std::unique_ptr<sf::Sprite> _sprite;

	float _angle;
	float _angularSpeed;
	float _alpha;
	float _alphaSpeed;
	sf::Time _timeToLeave;

public:
	Particle() = default;
	Particle(Particle&&) noexcept = default;
	~Particle() = default;

	Particle& operator= (Particle&&) noexcept = default;
};








class Scenario
{
private:
	LevelProperties _properties;
	PlayerId _playerId;
	ScenarioState _state;
	sf::IntRect _bounds;
	BubbleBoard _board;
	RNG _rand;
	//BubbleColorRandomizer _colors;
	//BubbleGenerator _bgen;
	//ScenarioArrow _arrow;
	//ScenarioScore _score;
	//ScenarioTimer _timer;
	std::list<std::shared_ptr<Bubble>> _movingBubbles;
	std::list<std::shared_ptr<Bubble>> _fallingBubbles;
	//std::list<HomingBubble> _homingBubbles;
	std::list<std::shared_ptr<sf::Sprite>> _animatedElements;
};

