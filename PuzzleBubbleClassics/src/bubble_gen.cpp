#include "bubble_gen.h"


void RandomBubbleModelSelector::build(const RandomBubbleModelSelectorScores& scores)
{
	_models = scores._models;
	computeScore();
}

std::shared_ptr<BubbleModel> RandomBubbleModelSelector::select(RNG& rand) const
{
	if (_score == 0)
		BubbleModelManager::instance().getDefaultModel();

	computeScore();

	Int64 value = Int64(rand(0, RNG::ResultType(_score)));
	for (const auto& model : _models)
	{
		value -= model.second;
		if (value < 0)
			return BubbleModelManager::instance().get(model.first);
	}

	return BubbleModelManager::instance().getDefaultModel();
}

RandomBubbleModelSelector::ScoreType RandomBubbleModelSelector::getModelScore(const std::shared_ptr<BubbleModel>& model) const
{
	if (model == nullptr || !model->isLoaded())
		return 0;

	auto it = _models.find(model->getName());
	if (it != _models.end())
		return it->second;
	return 0;
}

void RandomBubbleModelSelector::computeScore() const
{
	if (_recompute)
	{
		RNG::ResultType score = 0;
		for (const auto& model : _models)
			score += model.second;

		_score = score;
		_recompute = false;
	}
}




void BubbleColorRandomizer::updateCache()
{
	_cache.clear();
	for (BubbleColor color : AllColors)
		if (isColorEnabled(color))
			_cache.push_back(color);
}

std::optional<BubbleColor> BubbleColorRandomizer::generate(const BubbleBoard& board)
{
	/* TODO */
	return {};
}




void BubbleGenerator::build(Scenario& scenario)
{
	/* TODO */
}

BubbleColor BubbleGenerator::selectColor(bool toArrow)
{
	if (toArrow)
	{
		std::optional<BubbleColor> color = {} /* TODO */;
		if (!color.has_value())
			return _lastColor;

		_lastColor = *color;
		return *color;
	}
	else return _colors.generate();
}
