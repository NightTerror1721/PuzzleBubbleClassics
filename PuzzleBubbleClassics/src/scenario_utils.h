#pragma once

#include "particle.h"

class Scenario;


class RemoteTimes : public Particle
{
private:
	static constexpr std::string_view FontName = "bubble2";

private:
	Reference<Scenario> _scenario;
	unsigned int _count = 0;
	sf::Text _countText;
	sf::Text _timesText;
	float _max = .5f;

	std::shared_ptr<sf::Font> _font;

public:
	RemoteTimes() = default;
	RemoteTimes(const RemoteTimes&) = delete;
	RemoteTimes(RemoteTimes&&) noexcept = default;
	~RemoteTimes() = default;

	RemoteTimes& operator= (const RemoteTimes&) = delete;
	RemoteTimes& operator= (RemoteTimes&&) noexcept = default;

public:
	constexpr void clear() { _count = 0; }

	constexpr unsigned int getCount() const { return _count; }

public:
	void init(Scenario& scenario);

	void addTime();

	void update(const sf::Time& elapsedTime) override;
	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override;

private:
	void updateTextsAlphas();
};



class BubbleBoardEdges : public Renderable
{
private:
	static constexpr float Size = 27;
	static constexpr int NumSprites = 8;

private:
	Reference<Scenario> _scenario;
	std::array<sf::RectangleShape, NumSprites> _sprites;

public:
	BubbleBoardEdges() = default;
	BubbleBoardEdges(const BubbleBoardEdges&) = delete;
	BubbleBoardEdges(BubbleBoardEdges&&) noexcept = default;
	~BubbleBoardEdges() = default;

	BubbleBoardEdges& operator= (const BubbleBoardEdges&) = delete;
	BubbleBoardEdges& operator= (BubbleBoardEdges&&) noexcept = default;

public:
	void init(Scenario& scenario);

	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override;

private:
	//static void initEdge(sf::RectangleShape& edge, std::string_view textureName, )
};
