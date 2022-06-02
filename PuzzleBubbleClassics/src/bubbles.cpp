#include "bubbles.h"


const BubbleColor BubbleColor::Colorless = BubbleColorCode::Colorless;
const BubbleColor BubbleColor::Red = BubbleColorCode::Red;
const BubbleColor BubbleColor::Orange = BubbleColorCode::Orange;
const BubbleColor BubbleColor::Yellow = BubbleColorCode::Yellow;
const BubbleColor BubbleColor::Green = BubbleColorCode::Green;
const BubbleColor BubbleColor::Blue = BubbleColorCode::Blue;
const BubbleColor BubbleColor::Purple = BubbleColorCode::Purple;
const BubbleColor BubbleColor::Gray = BubbleColorCode::Gray;
const BubbleColor BubbleColor::Black = BubbleColorCode::Black;
const BubbleColor BubbleColor::Multicolor = BubbleColorCode::Multicolor;

bool BubbleColor::matches(BubbleColor other) const
{
	using enum BubbleColorCode;
	switch (_code)
	{
		case Colorless:
			return false;

		case Multicolor:
			return other._code != Colorless;

		default: switch (other._code)
		{
			case Colorless:
				return false;

			case Multicolor:
				return true;

			default:
				return _code == other._code;
		}
	}
}





void BubbleModel::callOnInit(const std::shared_ptr<Bubble>& bubble, BubbleColor color, bool editorMode)
{
	if (onInit)
		(*onInit)(bubble.get(), color.asNumber(), editorMode);
}

void BubbleModel::callOnCollide(const std::shared_ptr<Bubble>& self, const std::shared_ptr<Bubble>& other)
{
	if (onCollide)
		(*onCollide)(self.get(), other.get());
}

void BubbleModel::callOnInserted(const std::shared_ptr<Bubble>& self)
{
	if (onInserted)
		(*onInserted)(self.get());
}

void BubbleModel::callOnExplode(const std::shared_ptr<Bubble>& self)
{
	if (onExplode)
		(*onExplode)(self.get());
}

void BubbleModel::callOnNeighborInserted(const std::shared_ptr<Bubble>& self, const std::shared_ptr<Bubble>& other)
{
	if (onNeighborInserted)
		(*onNeighborInserted)(self.get(), other.get());
}

void BubbleModel::callOnNeighborExplode(const std::shared_ptr<Bubble>& self, const std::shared_ptr<Bubble>& other)
{
	if (onNeighborExplode)
		(*onNeighborExplode)(self.get(), other.get());
}

void BubbleModel::lua_SetEvent(std::unique_ptr<LuaRef>& event, const LuaRef& func, LuaState* state)
{
	if (!func.isFunction() && !func.isNil())
		lua::error(state, "Expected valid function on event setting.");
	
	event = std::unique_ptr<LuaRef>(new LuaRef(state, func));
}

LuaRef BubbleModel::lua_GetEvent(const std::unique_ptr<LuaRef>& event, LuaState* state) const
{
	if (event)
		return *event;
	return LuaRef(state);
}





BounceEdge BouncingBounds::check()
{
	Vector2f pos = _bubble.getPosition();
	Vector2f speed = _bubble.getSpeed();
	if (_top && pos.y - Bubble::radius <= _bounds.top)
	{
		pos.y = static_cast<float>(_bounds.top + Bubble::radius);
		speed.y *= -1;
		_bubble.setPosition(pos);
		_bubble.setSpeed(speed);
		return BounceEdge::Top;
	}
	if (_bottom && pos.y + Bubble::radius >= _bounds.top + _bounds.height)
	{
		pos.y = static_cast<float>(_bounds.top + _bounds.height - Bubble::radius);
		speed.y *= -1;
		_bubble.setPosition(pos);
		_bubble.setSpeed(speed);
		return BounceEdge::Bottom;
	}
	if (pos.x - Bubble::radius <= _bounds.left)
	{
		pos.x = static_cast<float>(_bounds.left + Bubble::radius);
		speed.x *= -1;
		_bubble.setPosition(pos);
		_bubble.setSpeed(speed);
		return BounceEdge::Left;
	}
	if (pos.x + Bubble::radius >= _bounds.left + _bounds.width)
	{
		pos.x = static_cast<float>(_bounds.left + _bounds.width - Bubble::radius);
		speed.x *= -1;
		_bubble.setPosition(pos);
		_bubble.setSpeed(speed);
		return BounceEdge::Right;
	}
	return BounceEdge::None;
}







Bubble::Bubble(const std::shared_ptr<BubbleModel>& model, TextureManager& texs) :
	_model(model),
	_exploited{ false },
	_speed{},
	_acceleration{},
	_allocPosition{},
	_allocCell{},
	_floatingCheckPhase{ false },
	_color{ BubbleColor::Colorless },
	_sprite{},
	_bounce{ *this }
{}

Bubble::~Bubble() {}

const std::shared_ptr<BubbleModel>& Bubble::getModel() const { return _model; }

bool Bubble::hasExploited() const { return _exploited; }

void Bubble::explode()
{
	//TODO: Implement
}

void Bubble::setSpeed(const Vector2f& speed) { _speed = speed; }
const Vector2f& Bubble::getSpeed() const { return _speed; }

void Bubble::setAcceleration(const Vector2f& acceleration) { _acceleration = acceleration; }
const Vector2f& Bubble::getAcceleration() const { return _acceleration; }

void Bubble::translate(const Vector2f& dp) { setPosition(getPosition() + dp); }
void Bubble::translate(float dx, float dy) { translate({ dx, dy }); }
void Bubble::move(const Vector2f& speed, const Vector2f& acceleration) { _speed = speed; _acceleration = acceleration; }

BubbleColor Bubble::getColor() const { return _color; }

bool Bubble::colorMatch(const std::shared_ptr<Bubble>& other) const { return _color.matches(other->_color); }

AnimatedSprite* Bubble::getSprite() { return &_sprite; }
const AnimatedSprite* Bubble::getSprite() const { return &_sprite; }


/* Model functions */
Int8 Bubble::getResistence() const { return _model->resistence; }
bool Bubble::isIndestructible() const { return _model->resistence < 0; }
bool Bubble::isFloating() const { return _model->floating; }
bool Bubble::destroyInBottom() const { return _model->destroyInBottom; }
bool Bubble::requireDestroyToClear() const { return _model->requireDestroyToClear; }
float Bubble::getPointsOfTurnsToDown() const { return _model->pointsOfTurnsToDown; }







std::shared_ptr<BubbleModel> BubbleModelManager::createModel(const String& name)
{
	auto it = _models.find(name);
	if (it != _models.end())
		return it->second;

	auto res = _models.insert({ name, std::shared_ptr<BubbleModel>(new BubbleModel()) });
	if (res.second)
		return res.first->second;
	return nullptr;
}

std::shared_ptr<BubbleModel> BubbleModelManager::getModel(const String& name) const
{
	auto it = _models.find(name);
	if (it != _models.end())
		return it->second;
	return nullptr;
}

bool BubbleModelManager::hasModel(const String& name) const
{
	return _models.contains(name);
}

std::shared_ptr<BubbleModel> BubbleModelManager::getDefaultModel() const
{
	return _defaultModel;
}

void BubbleModelManager::setDefaultModel(const String& name)
{
	auto model = getModel(name);
	if (model)
		_defaultModel = model;
}

std::shared_ptr<Bubble> BubbleModelManager::createBubble(TextureManager& textures, bool editorMode, const String& modelName, BubbleColor color) const
{
	auto model = getModel(modelName);
	if (!model)
		return nullptr;

	std::shared_ptr<Bubble> bubble = std::shared_ptr<Bubble>(new Bubble(model, textures));
	model->callOnInit(bubble, color, editorMode);

	return bubble;
}

BubbleModelManager BubbleModelManager::instance;







void RandomBubbleModelSelector::setModelScore(const String& modelName, UInt16 score)
{
	if (BubbleModelManager::instance.hasModel(modelName))
	{
		if (score == 0)
			_models.erase(modelName);
		else
			_models[modelName] = score;
		_recompute = true;
	}
}

UInt16 RandomBubbleModelSelector::getModelScore(const String& modelName) const
{
	auto it = _models.find(modelName);
	if (it != _models.end())
		return it->second;
	return 0;
}

std::shared_ptr<BubbleModel> RandomBubbleModelSelector::selectModel(RNG& rand) const
{
	if (_score == 0)
		return BubbleModelManager::instance.getDefaultModel();

	computeScore();

	std::shared_ptr<BubbleModel> result;
	Int64 value = static_cast<Int64>(rand(0, _score));
	for (const auto& model : _models)
	{
		value -= model.second;
		if (value < 0)
		{
			result = BubbleModelManager::instance.getModel(model.first);
			break;
		}
	}

	if (result)
		return result;
	return BubbleModelManager::instance.getDefaultModel();
}

void RandomBubbleModelSelector::computeScore() const
{
	if (_recompute)
	{
		RNG::RandomValue score = 0;
		for (const auto& model : _models)
			score += model.second;

		_score = score;
		_recompute = false;
	}
}
