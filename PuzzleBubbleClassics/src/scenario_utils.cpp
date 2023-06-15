#include "scenario_utils.h"

#include "scenario.h"
#include "font.h"


void RemoteTimes::init(Scenario& scenario)
{
//	_scenario = std::addressof(scenario);
	_font = FontManager::instance().get(FontName.data());

	_timesText.setFont(*_font);
	_timesText.setString("times");
	_timesText.setFillColor({ 220, 200, 16 });
	_timesText.setOutlineColor({ 0, 16, 196 });
	_timesText.setOutlineThickness(1.f);
	
	_countText.setFont(*_font);
	_countText.setString("0");
	_countText.setFillColor({ 16, 200, 16 });
	_countText.setOutlineColor({ 0, 16, 196 });
	_countText.setOutlineThickness(1.f);
	
	setTimeToLeave(sf::Time::Zero);
}

void RemoteTimes::addTime()
{
	_count++;
	setTimeToLeave(sf::seconds(1.5f));
	setAlpha(1);
	setSpeed({ 0, -480 });
	setAcceleration({ 0, 1440 });
	_timesText.setString(_count == 1 ? " time" : " times");
	_countText.setString(std::to_string(_count));

//	Rectangle bounds = scenario.getBounds();
//	position.set(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2);

	auto countBounds = _countText.getLocalBounds();
	auto timesBounds = _timesText.getLocalBounds();
	const auto& position = getPosition();

	float width = countBounds.width + timesBounds.width;
	float height = countBounds.height + timesBounds.height;
	float x = getPosition().x - (width / 2.f);
	float y = getPosition().y - (height / 2.f);

	_countText.setPosition(x, y);
	_timesText.setPosition(x + countBounds.width, y);

	_countText.setOrigin(0, 0);
	_timesText.setOrigin(0, 0);

	updateTextsAlphas();
}

void RemoteTimes::update(const sf::Time& elapsedTime)
{
	if (isDead())
		return;

	Particle::update(elapsedTime);

	if (getTimeToLeave().asSeconds() < _max)
		setAlpha(getTimeToLeave().asSeconds() / _max);

	if (getAcceleration().y != 0)
	{
		const auto& acc = getAcceleration();
		const auto& speed = getSpeed();
		if ((acc.y < 0 && speed.y <= 0) || (acc.y > 0 && speed.y >= 0))
		{
			setSpeed({ speed.x, 0 });
			setAcceleration({ acc.x, 0 });
		}
		else
		{
			setSpeed({ speed.x, -speed.y });
			setAcceleration({ acc.x, -acc.y });
		}
	}

	updateTextsAlphas();
}

void RemoteTimes::render(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	if (isDead())
		return;

	rs.transform *= getTransform();
	rs.blendMode = sf::BlendAlpha;
	canvas.draw(_countText, rs);
	canvas.draw(_timesText, rs);
}

void RemoteTimes::updateTextsAlphas()
{
	auto color = _countText.getFillColor();
	color.a = Uint8(getAlpha() * 255);
	_countText.setFillColor(color);

	color = _countText.getOutlineColor();
	color.a = Uint8(getAlpha() * 255);
	_countText.setOutlineColor(color);


	color = _timesText.getFillColor();
	color.a = Uint8(getAlpha() * 255);
	_timesText.setFillColor(color);

	color = _timesText.getOutlineColor();
	color.a = Uint8(getAlpha() * 255);
	_timesText.setOutlineColor(color);
}
