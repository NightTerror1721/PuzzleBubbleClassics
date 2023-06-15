#include "debug_grid.h"

#include "game_controller.h"


void DebugGrid::render(sf::RenderTarget& canvas, sf::RenderStates rs)
{
	sf::RectangleShape rect;
	renderVerticalLines(canvas, rs, rect);
	renderHorizontalLines(canvas, rs, rect);
}

void DebugGrid::renderVerticalLines(sf::RenderTarget& canvas, sf::RenderStates rs, sf::RectangleShape& rect)
{
	auto transf = rs.transform;
	rect.setSize({ 2, float(GameController::CanvasHeight) });
	rect.setPosition(-1, 0);

	for (float i = _size; i < float(GameController::CanvasWidth); i += _size)
	{
		rect.move(_size, 0);
		canvas.draw(rect, rs);
	}
}

void DebugGrid::renderHorizontalLines(sf::RenderTarget& canvas, sf::RenderStates rs, sf::RectangleShape& rect)
{
	auto transf = rs.transform;
	rect.setSize({ float(GameController::CanvasWidth), 2 });
	rect.setPosition(0, -1);

	for (float i = _size; i < float(GameController::CanvasHeight); i += _size)
	{
		rect.move(0, _size);
		canvas.draw(rect, rs);
	}
}
