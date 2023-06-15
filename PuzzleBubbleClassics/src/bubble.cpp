#include "bubble.h"

#include "data.h"


EdgeBounce BouncingBounds::check()
{
	return EdgeBounce::None;
}


BubbleModelManager BubbleModelManager::Instance;

std::shared_ptr<BubbleModel> BubbleModelManager::load(const std::string_view name)
{
	std::optional<Path> path = DataPool::instance().findFilePath(ResourceDirectoryType::Bubbles, name);
	if (!path.has_value())
		return nullptr;

	auto model = loadTemplate(path.value());
	if (model != nullptr && model->onIsDefaultModel())
	{
		if (_defaultModel != nullptr)
			logger::error("Duplicated default model. '{}' and '{}'.", model->getName(), _defaultModel->getName());
		else
			_defaultModel = model;
	}
	return model;
}

void BubbleModelManager::loadAllModels()
{
	std::unordered_map<std::string, Path> models;
	DataPool::instance().forEachDirectoryPath(ResourceDirectoryType::Bubbles, [&models](const Path& path) {
		if (utils::path::hasExtension(path, ".lua"))
			models.insert({ utils::path::getFileName(path, false), path });
	});

	for(const auto& model : models)
		loadTemplate(model.second);
}



Bubble::Bubble() : _bounce(*this) {}

std::shared_ptr<Bubble> Bubble::make(const std::shared_ptr<BubbleModel>& model, BubbleColor color, bool editorMode)
{
	if (model == nullptr)
		return nullptr;

	std::shared_ptr<Bubble> bubble = std::shared_ptr<Bubble>(new Bubble());
	bubble->_model = model;
	model->onConstruct(bubble, color, editorMode);

	return bubble;
}

std::shared_ptr<Bubble> Bubble::make(std::string_view model, BubbleColor color, bool editorMode)
{
	return make(BubbleModelManager::instance().get(model.data()), color, editorMode); 
}


void Bubble::render(sf::RenderTarget& target, sf::RenderStates rs)
{
	SpriteMotionObject::render(target, rs);
}

void Bubble::update(const sf::Time& elapsedTime)
{
	SpriteMotionObject::update(elapsedTime);
}
