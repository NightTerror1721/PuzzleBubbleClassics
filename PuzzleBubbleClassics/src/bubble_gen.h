#pragma once

#include "level.h"

#include <optional>


class Scenario;
class BubbleBoard;

class RandomBubbleModelSelector
{
public:
	using ScoreType = utils::level::RandomBubbleModelSelectorScoreType;

public:
	static constexpr ScoreType MinScore = utils::level::RandomBubbleModelSelectorMinScore;
	static constexpr ScoreType MaxScore = utils::level::RandomBubbleModelSelectorMaxScore;

private:
	std::unordered_map<std::string, ScoreType> _models;
	mutable RNG::ResultType _score = 0;
	mutable bool _recompute = true;

public:
	RandomBubbleModelSelector() = default;
	RandomBubbleModelSelector(const RandomBubbleModelSelector&) = default;
	RandomBubbleModelSelector(RandomBubbleModelSelector&&) = default;
	~RandomBubbleModelSelector() = default;

	RandomBubbleModelSelector& operator= (const RandomBubbleModelSelector&) = default;
	RandomBubbleModelSelector& operator= (RandomBubbleModelSelector&&) = default;

public:
	void build(const RandomBubbleModelSelectorScores& scores);

	std::shared_ptr<BubbleModel> select(RNG& rand) const;

	ScoreType getModelScore(const std::shared_ptr<BubbleModel>& model) const;

private:
	void computeScore() const;

public:
	inline ScoreType getModelScore(const std::string& modelName) const
	{
		return getModelScore(BubbleModelManager::instance().get(modelName));
	}

	inline void forEach(const std::function<void(const std::string&, ScoreType)>& action) const
	{
		for (const auto& entry : _models)
			action(entry.first, entry.second);
	}
};



class BubbleColorRandomizer
{
private:
	static constexpr auto AllColors = BubbleColor::all();

private:
	RNG _rand;
	AvailableColorsTable _availableColors;
	std::vector<BubbleColor> _cache;

public:
	BubbleColorRandomizer() = default;
	BubbleColorRandomizer(const BubbleColorRandomizer&) = default;
	BubbleColorRandomizer(BubbleColorRandomizer&&) = default;
	~BubbleColorRandomizer() = default;

	BubbleColorRandomizer& operator= (const BubbleColorRandomizer&) = default;
	BubbleColorRandomizer& operator= (BubbleColorRandomizer&&) = default;

public:
	constexpr void setRand(const RNG& rand) { _rand = rand; }
	constexpr void setRand(RNG&& rand) { _rand = std::move(rand); }

	inline void setAvailableColors(AvailableColorsTable availableColors)
	{
		_availableColors = availableColors;
		updateCache();
	}

	inline void setColorEnabled(BubbleColor color, bool enabled)
	{
		_availableColors.setEnabled(color, enabled);
		updateCache();
	}

	constexpr bool isColorEnabled(BubbleColor color) const { return _availableColors.isEnabled(color); }

	inline BubbleColor generate() { return _cache.empty() ? BubbleColor::Default : _rand.randomEntry(_cache); }

	inline BubbleColor operator() () { return generate(); }
	inline std::optional<BubbleColor> operator() (const BubbleBoard& board) { return generate(board); }

public:
	std::optional<BubbleColor> generate(const BubbleBoard& board);

private:
	void updateCache();
};



class BubbleGenerator
{
private:
	Reference<Scenario> _scenario;
	RNG _arrowRand;
	RNG _boardRand;
	BubbleColorRandomizer _colors;
	RandomBubbleModelSelector _arrowModels;
	RandomBubbleModelSelector _boardModels;
	BubbleColor _lastColor;

public:
	BubbleGenerator() = default;
	BubbleGenerator(const BubbleGenerator&) = default;
	BubbleGenerator(BubbleGenerator&&) = default;
	~BubbleGenerator() = default;

	BubbleGenerator& operator= (const BubbleGenerator&) = default;
	BubbleGenerator& operator= (BubbleGenerator&&) = default;

public:
	constexpr RandomBubbleModelSelector& getArrowModelSelector() { return _arrowModels; }
	constexpr const RandomBubbleModelSelector& getArrowModelSelector() const { return _arrowModels; }

	constexpr RandomBubbleModelSelector& getBoardModelSelector() { return _boardModels; }
	constexpr const RandomBubbleModelSelector& getBoardModelSelector() const { return _boardModels; }

	inline std::shared_ptr<Bubble> makeBubble(const std::shared_ptr<BubbleModel>& model, bool toArrow)
	{
		return Bubble::make(model, selectColor(toArrow), false);
	}

	inline std::shared_ptr<Bubble> makeBubble(bool toArrow)
	{
		return makeBubble(modelSelector(toArrow).select(rand(toArrow)), toArrow);
	}

	inline std::shared_ptr<Bubble> makeBubble(const MetaBubble& metaBubble, bool toArrow)
	{
		if (metaBubble.hasRandomColor())
			return makeBubble(metaBubble.getModel(), toArrow);
		return Bubble::make(metaBubble.getModel(), metaBubble.getColor());
	}

public:
	void build(Scenario& scenario);

private:
	BubbleColor selectColor(bool toArrow);

private:
	constexpr RNG& rand(bool toArrow) { return toArrow ? _arrowRand : _boardRand; }
	constexpr RandomBubbleModelSelector& modelSelector(bool toArrow) { return toArrow ? _arrowModels : _boardModels; }
};
