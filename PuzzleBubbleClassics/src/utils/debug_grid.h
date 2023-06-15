#pragma once

#include "object_basics.h"


class DebugGrid : public Renderable
{
private:
	float _size = 20;

public:
	DebugGrid() = default;
	DebugGrid(const DebugGrid&) = default;
	DebugGrid(DebugGrid&&) noexcept = default;
	~DebugGrid() = default;

	DebugGrid& operator= (const DebugGrid&) = default;
	DebugGrid& operator= (DebugGrid&&) = default;

public:
	constexpr void setSize(float size) { _size = std::max(1.f, size < 0 ? -size : size); }
	constexpr float getSize() const { return _size; }

public:
	void render(sf::RenderTarget& canvas, sf::RenderStates rs) override;

private:
	void renderVerticalLines(sf::RenderTarget& canvas, sf::RenderStates rs, sf::RectangleShape& rect);
	void renderHorizontalLines(sf::RenderTarget& canvas, sf::RenderStates rs, sf::RectangleShape& rect);
};
