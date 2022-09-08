#include "bubble_board.h"



void BubbleBoardCell::create(BubbleBoardRow& board, ColumnIndex column)
{
	if (isValid())
		removeBubble();

	_owner = std::addressof(board);
	_column = column;
}






std::strong_ordering BubbleBoardRow::operator<=> (const BubbleBoardRow& right) const
{
	auto cmp = _owner <=> right._owner;
	if (cmp == std::strong_ordering::equal)
		return _row <=> right._row;
	return cmp;
}

void BubbleBoardRow::create(BubbleBoard& board, RowIndex row, BoardColumnStyle columns, bool little)
{
	_owner = std::addressof(board);
	_row = row;
	_little = little;
	_cells = { levels::styleToColumns(columns) - (little ? 1 : 0) };
}

bool BubbleBoardRow::_unsafeRemoveBubble(BubbleBoardCell& cell)
{
	if (!cell.isEmpty())
	{
		cell->removeCell();
		cell.removeBubble();
		return true;
	}
	return false;
}

ColumnCount BubbleBoardRow::getColumns() const
{
	if (!isValid())
		return 0;

	if (_little)
		return levels::styleToColumns(_owner->getColumns()) - 1;
	return levels::styleToColumns(_owner->getColumns());
}

int BubbleBoardRow::clear()
{
	int count = 0;
	for (auto& cell : _cells)
	{
		if (_unsafeRemoveBubble(cell))
			count++;
	}
	_bubbleCount = 0;

	return count;
}

void BubbleBoardRow::setBubble(ColumnIndex column, const std::shared_ptr<Bubble>& bubble)
{
	/* TODO */
}

std::shared_ptr<Bubble> BubbleBoardRow::removeBubble(ColumnIndex column)
{
	if (column < getColumns())
	{
		auto& cell = _cells[column];
		if (cell.hasBubble())
		{
			auto bubble = cell.getBubble();
			_unsafeRemoveBubble(cell);
			_bubbleCount--;
			return bubble;
		}
	}
	return nullptr;
}

void BubbleBoardRow::descend()
{
	for (const auto& cell : _cells)
	{
		if (cell.hasBubble())
			cell->translate(0, Bubble::getHitboxHeight());
	}
}

bool BubbleBoardRow::hasInvalidBottomBubble() const
{
	for (auto& cell : _cells)
	{
		if (cell.hasBubble() && !cell->destroyInBottom())
			return true;
	}
	return false;
}








