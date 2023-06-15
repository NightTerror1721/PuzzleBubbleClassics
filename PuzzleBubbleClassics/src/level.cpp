#include "level.h"


Uint32 MetaGoals::getBubbleGoals(const MetaBubble& bubble)
{
	auto it = _bubbleGoals.find(bubble);
	if (it == _bubbleGoals.end())
		return 0;
	return it->second;
}

void MetaGoals::setBubbleGoals(const MetaBubble& bubble, Uint32 timesToDestroy)
{
	if (timesToDestroy == 0)
		_bubbleGoals.erase(bubble);
	else
	{
		auto it = _bubbleGoals.find(bubble);
		if (it == _bubbleGoals.end())
			_bubbleGoals.insert({ bubble, timesToDestroy });
		else
			it->second = timesToDestroy;
	}
}




void RandomBubbleModelSelectorScores::setModelScore(const std::shared_ptr<BubbleModel>& model, ScoreType score)
{
	if (model != nullptr && model->isLoaded())
	{
		if (score == 0)
			_models.erase(model->getName());
		else
		{
			auto it = _models.find(model->getName());
			if (it == _models.end())
				_models.insert({ model->getName(), score });
			else
				it->second = score;
		}
	}
}

RandomBubbleModelSelectorScores::ScoreType RandomBubbleModelSelectorScores::getModelScore(const std::shared_ptr<BubbleModel>& model) const
{
	if (model == nullptr || !model->isLoaded())
		return 0;

	auto it = _models.find(model->getName());
	if (it != _models.end())
		return it->second;
	return 0;
}
