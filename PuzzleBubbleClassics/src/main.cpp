#include "data.h"
#include "game_controller.h"
#include "bubble.h"
#include "level.h"
#include "font.h"
#include "data.h"
#include "utils/rng.h"
#include "scenario_utils.h"
#include "utils/debug_grid.h"
#include "utils/angle.h"


class TestActivity : public GameActivity
{
private:
	sf::Text _text;
	std::shared_ptr<sf::Font> _font;
	sf::Time _remaining;
	RNG _rng;

	DebugGrid _dbgrid;

public:
	void init() override
	{
		_font = FontManager::instance().get("bubble2");
		_text.setFont(*_font);
		_text.setCharacterSize(50);
		_text.setOutlineThickness(1);
		_text.setOutlineColor({ 0, 0, 255 });
		_text.setFillColor({ 16, 220, 16 });
		_text.setPosition(500, 200);
		_remaining = sf::Time::Zero;
	}

	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override
	{
		auto bounds = _text.getLocalBounds();
		canvas.draw(_text, rs);
		//_dbgrid.render(canvas, rs);
	}

	void update(const sf::Time& elapsedTime) override
	{
		_text.rotate(10.f * elapsedTime.asSeconds());
		if (_remaining <= sf::Time::Zero)
		{
			auto value = _rng(0, 10000);
			_text.setString(std::format("Random number: {}", value));
			auto bounds = _text.getLocalBounds();
			_text.setOrigin({
				bounds.width / 2.f,
				bounds.height / 2.f
			});

			_remaining = sf::milliseconds(1000.f + _rng(0, 2000));
		}
		else _remaining -= elapsedTime;
	}
};


int main(int argc, char** argv)
{
	auto angle = 180_deg;

	DataPool::instance().load();

	BubbleModelManager::instance().loadAllModels();

	auto a = GameController::instance().createActivity<TestActivity>();
	GameController::instance().start();

	HiddenBubbleContainerType type;
	type.discrete = true;

	return 0;
}
