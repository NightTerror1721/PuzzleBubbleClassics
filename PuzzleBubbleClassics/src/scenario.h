#pragma once

#include "scenario_utils.h"
#include "bubble_board.h"
#include "particle.h"
#include "arrow.h"

#include <forward_list>


enum class ScenarioState
{
	InGame,
	Paused,
	Winning,
	Losing,
	Won,
	Lost
};

namespace utils::scenario
{
	constexpr bool isFinalState(ScenarioState state)
	{
		return state == ScenarioState::Won || state == ScenarioState::Lost;
	}

	constexpr bool isRunningState(ScenarioState state) { return !isFinalState(state); }

	constexpr bool isActiveRunningState(ScenarioState state) { return state == ScenarioState::InGame; }

	constexpr bool isPasiveRunningState(ScenarioState state)
	{
		return state == ScenarioState::Winning || state == ScenarioState::Losing || state == ScenarioState::Paused;
	}

	constexpr bool isFinalizingState(ScenarioState state)
	{
		return state == ScenarioState::Winning || state == ScenarioState::Losing;
	}
}



class Scenario : public GameObject, public sf::Transformable
{
public:
	using State = ScenarioState;

private:
	LevelProperties _properties;
	PlayerId _playerId;
	State _state;
	sf::Vector2f _size;
	BubbleBoard _board;
	RNG _rand;
	std::shared_ptr<BubbleColorRandomizer> _colors;
	std::shared_ptr<BubbleGenerator> _bgen;
	Arrow _arrow;
//	Score _score; TODO
//	Timer _timer; TODO
	std::forward_list<std::shared_ptr<Bubble>> _moving;
	std::forward_list<std::shared_ptr<Bubble>> _falling;
	std::forward_list<std::shared_ptr<Bubble>> _remoteMoving;
	std::forward_list<std::shared_ptr<AnimationObject>> _animations;
	std::forward_list<std::shared_ptr<Particle>> _particles;
	RemoteTimes _remoteTimes;

public:

public:
	constexpr const LevelProperties& getProperties() const { return _properties; }
	constexpr PlayerId getPlayerId() const { return _playerId; }
	inline const sf::FloatRect& getLocalBounds() const { return { getPosition().x, getPosition().y, _size.x, _size.y }; }
	constexpr State getState() const { return _state; }
	constexpr BubbleBoard& getBoard() { return _board; }
	constexpr const BubbleBoard& getBoard() const { return _board; }
	inline const std::shared_ptr<BubbleColorRandomizer>& getColorGenerator() const { return _colors; }
	inline const std::shared_ptr<BubbleGenerator>& getBubbleGenerator() const { return _bgen; }
	constexpr Arrow& getArrow() { return _arrow; }
	constexpr const Arrow& getArrow() const { return _arrow; }
	constexpr RemoteTimes& getRemoteTimes() { return _remoteTimes; }
	constexpr const RemoteTimes& getRemoteTimes() const { return _remoteTimes; }

};
