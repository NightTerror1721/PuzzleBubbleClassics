#include "arrow.h"

#include "scenario.h"


void ArrowTextures::init(const Arrow& arrow)
{
	_arrow = std::addressof(arrow);
	_texture = loadSprite("arrow.arrow", "arrow/arrow.png");
	_base = loadSprite("arrow.arrow", "arrow/arrow.png");
	_base2 = loadSprite("arrow.arrow", "arrow/arrow.png");
	_lever = loadSprite("arrow.arrow", "arrow/arrow.png");
	for (int i = 0; i < _gears.size(); ++i)
	{
		_gears[i] = loadSprite(
			std::string("arrow.gear") + std::to_string(i),
			std::string("arrow/gear") + std::to_string(i + 1) + ".png"
		);
	}

	auto arrowBounds = _arrow->getLocalBounds();
	setPosition(
		BoundsBase.x + arrowBounds.width / 2.f,
		BoundsBase.y
	);
	setSize(BoundsBase);

	_base.setPosition(0, 0);
	_base.setSize(getSize());

	_base2.setPosition(0, 0);
	_base2.setSize(getSize());

	_lever.setPosition(
		pbx(LeverDelta.x),
		pby(LeverDelta.y)
	);
	_lever.setSize(
		LeverSize.width,
		LeverSize.height
	);

	situateGear(0);
	situateGear(1);
	situateGear(2);

	_gears[3].setPosition(
		(arrowBounds.width / 2.f) - ((Bubble::Diameter + Gear3Offset) / 2.f),
		(arrowBounds.height / 2.f) - ((Bubble::Diameter + Gear3Offset) / 2.f)
	);
	_gears[3].setSize(
		Bubble::Diameter + Gear3Offset,
		Bubble::Diameter + Gear3Offset
	);
}

void ArrowTextures::drawBase(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	// TODO //
}

void ArrowTextures::drawArrow(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	// TODO //
}

Sprite ArrowTextures::loadSprite(std::string_view textureKey, std::string_view textureFilename)
{
	auto texture = TextureManager::instance().get(textureKey.data());
	if (texture == nullptr)
	{
		if (!TextureManager::instance().load(Path(textureFilename), textureKey.data()))
		{
			logger::error("Cannot load '{}' scenario.arrow texture.", textureFilename);
			return Sprite();
		}
		texture = TextureManager::instance().get(textureKey.data());
	}

	return Sprite::makeStatic(*texture);
}




void Arrow::init(Scenario& scenario)
{
	_scenario = std::addressof(scenario);

	auto scBounds = _scenario->getLocalBounds();
	setPosition(
		(scBounds.width / 2.f) + BoundsOffset.x,
		scBounds.height + BoundsOffset.y
	);
	setSize(BoundsOffset);
	
	_texturesBase.init(*this);
	_playerId = _scenario->getPlayerId();
	_bgen = _scenario->getBubbleGenerator();
//	_fireSound = TODO
	_enabledSwap = _scenario->getProperties().isBubbleSwapEnabled();

	_swap = RequestFlag::Ready;
	_slowly = false;
	_angle = 0;
	_lock = true;
	_lockFire = true;
	_moveSoundOffset = 0;
	_currentBubble = nullptr;
	_nextBubble = nullptr;
}

void Arrow::fireBubble()
{
	if (!_lockFire)
	{
		_doFire = false;
//		_currentBubble.fire(*_scenario, _angle, _fireSound); TODO
		_currentBubble = nullptr;
		_lockFire = true;
//		_scenario->stopTurnTimer(); TODO
	}
}


void Arrow::makeNext()
{
	_currentBubble = _nextBubble;
	_currentBubble->setPosition(getWidth() / 2.f, getHeight() / 2.f);
	_nextBubble = _bgen->makeBubble(true);
	_nextBubble->setPosition(
		_texturesBase.getPosition().x + 128.f - Bubble::Diameter / 4.f,
		_texturesBase.getPosition().y + 32.f - Bubble::Diameter / 2.f
	);
}

void Arrow::swapBubble()
{
	_swap = RequestFlag::Done;
	if (_enabledSwap && _lockFire && _currentBubble != nullptr && _nextBubble != nullptr)
	{
		auto oldCurrentPos = _currentBubble->getPosition();
		auto oldNextPos = _nextBubble->getPosition();

		std::swap(_currentBubble, _nextBubble);
		_currentBubble->setPosition(oldNextPos);
		_nextBubble->setPosition(oldCurrentPos);
	}
}
