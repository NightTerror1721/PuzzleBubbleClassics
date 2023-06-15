#include "bubble_board.h"


bool BubbleBoardCell::clear()
{
	if (_bubble == nullptr)
		return false;

	_bubble->setCell(nullptr);
	_bubble.reset();
	return true;
}



std::shared_ptr<Bubble> BubbleBoardRow::setBubble(ColumnIndex column, const std::shared_ptr<Bubble>& bubble)
{
	if (column >= getColumns())
	{
		logger::error("Attempt to insert bubble on row {} out of columns rage [{}, {}].", column, 0, (getColumns() - 1));
		return nullptr;
	}

	std::shared_ptr<Bubble> old = removeBubble(column);
	if (bubble != nullptr)
	{
		//bubble->setPosition()
		/* TODO */
		
		_bubbleCount++;
	}
	_cells[column].setBubble(bubble);
	return old;
}

std::shared_ptr<Bubble> BubbleBoardRow::removeBubble(ColumnIndex column)
{
	if (column >= getColumns())
	{
		logger::error("Attempt to remove bubble on row {} out of columns rage [{}, {}].", column, 0, (getColumns() - 1));
		return nullptr;
	}

	Cell& cell = _cells[column];
	if (!cell.isEmpty())
	{
		auto bubble = cell.getBubble();
		cell.clear();
		_bubbleCount--;
		return bubble;
	}
	return nullptr;
}

std::size_t BubbleBoardRow::clear()
{
	std::size_t count = 0;
	for (Cell& cell : _cells)
		if (cell.clear())
			count++;
	return count;
}

void BubbleBoardRow::descend()
{
	for (const Cell& cell : _cells)
		if (!cell.isEmpty())
			cell->translate(0, float(Bubble::HitboxHeight));
}

bool BubbleBoardRow::hasInvalidBottomBubble() const
{
	for (const Cell& cell : _cells)
		if (!cell.isEmpty() && !cell->destroyInBottom())
			return true;
	return false;
}

void BubbleBoardRow::build(BubbleBoard& board, RowIndex row, BoardColumnStyle columnStyle, bool little)
{
	_owner = std::addressof(board);
	_row = row;
	_cells.resize(utils::level::adaptColumnCountIfRowIsOdd(row, columnStyle));
	_little = little;

	ColumnCount len = ColumnCount(_cells.size());
	for (ColumnIndex i = 0; i < len; ++i)
		_cells[i].build(*this, i);
}




std::vector<std::shared_ptr<Bubble>> HideRow::makeBubblesRow(BubbleGenerator& bgen)
{
	if (!empty())
	{
		auto bubs = std::vector<std::shared_ptr<Bubble>>(_bubbles.size());
		std::size_t len = bubs.size();
		for (std::size_t i = 0; i < len; ++i)
			if(_bubbles[i].isValid())
				bubs[i] = bgen.makeBubble(_bubbles[i], false);
		return bubs;
	}
	return {};
}





void HideBubbleContainer::build(Scenario& scenario, BoardColumnStyle columnStyle)
{
	// TODO: _type = scenario.
	_columnStyle = utils::level::validateColumnStyle(columnStyle);
	// TODO: _bgen = scenario.
	// TODO: _rand = scenario.?.randomRNG();
	/* TODO:
		Uint32 mb = scenario.getLevelProperties();
		_maxBoards = mb < 1 ? -1 : mb;
	*/
	_boards.clear();
	_current.reset();
}

void HideBubbleContainer::fill(const std::vector<MetaBubbleBoard>& metaBoards)
{
	_boards.clear();
	_current.reset();
	if (metaBoards.empty())
		return;

	const Uint64 max = std::max(Uint64(metaBoards.size()), _maxBoards);
	decltype(_boards) aux;
	aux.reserve(max);

	for (Uint64 i = 0; i < max; ++i)
	{
		const MetaBubbleBoard& meta = metaBoards.at(i);
		auto board = std::make_unique<HideBoard>();
		for (Int64 row = Int64(HideBoard::MaxRows - 1); row >= 0; row--)
		{
			std::unique_ptr<HideRow> hrow = makeHideRow(meta, RowIndex(row), HideBoardId(i));
			board->addRow(std::move(hrow));
		}
		aux.push_back(std::move(board));
	}

	if (_type.isContinuous() && aux.size() > 1)
		aux.back()->trimTop();

	if (_type.isRandom())
	{
		std::size_t idx = std::size_t(_rand(RNG::ResultType(aux.size())));
		auto board = std::move(aux.at(idx));
		aux.erase(aux.begin() + idx);
		_boards.push_back(std::move(board));
	}
	else _boards = std::move(aux);
}

std::vector<std::vector<std::shared_ptr<Bubble>>> HideBubbleContainer::generate()
{
	if (empty())
		return {};
	checkNext();

	if (_type.isContinuous())
		return { _current->buildNextRow(*_bgen) };
	return _current->buildAllRows(*_bgen);
}

std::vector<std::vector<std::shared_ptr<Bubble>>> HideBubbleContainer::generateBoard()
{
	if (empty())
		return {};

	checkNext();
	return _current->buildAllRows(*_bgen);
}

std::vector<std::shared_ptr<Bubble>> HideBubbleContainer::generateRow()
{
	if (_type.isDiscrete())
	{
		logger::error("Cannot generate single row on discrete mode");
		return {};
	}

	if (empty())
		return {};

	checkNext();
	return _current->buildNextRow(*_bgen);
}

Uint32 HideBubbleContainer::getBubbleCount() const
{
	Uint32 count = 0;
	for (const auto& board : _boards)
		count += Uint32(board->count());
	if (_current != nullptr)
		count += Uint32(_current->count());
	return count;
}

void HideBubbleContainer::checkNext()
{
	if (_current == nullptr || _current->empty())
	{
		_current = std::move(_boards.front());
		_boards.erase(_boards.begin());
		if (_type.isEndless())
		{
			auto board = std::make_unique<HideBoard>();
			*board = *_current;
			_boards.push_back(std::move(board));
		}
	}
}

std::unique_ptr<HideRow> HideBubbleContainer::makeHideRow(const MetaBubbleBoard& board, RowIndex row, HideBoardId boardId)
{
	return {};
}
