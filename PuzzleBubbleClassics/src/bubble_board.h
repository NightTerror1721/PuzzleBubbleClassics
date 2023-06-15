#pragma once

#include "level.h"
#include "bubble_gen.h"

#include <queue>


class Scenario;
class BubbleBoard;
class BubbleBoardRow;
class BubbleBoardCell;

class HideBoard;
class HideBubbleContainer;


using HideBoardId = Uint32;


struct BubbleBoardCellPosition
{
	RowIndex row = 0;
	ColumnIndex column = 0;
};


class BubbleBoardCell
{
public:
	using Position = BubbleBoardCellPosition;

	friend BubbleBoardRow;

private:
	Reference<BubbleBoardRow> _owner = nullptr;
	ColumnIndex _column = 0;
	std::shared_ptr<Bubble> _bubble = nullptr;

public:
	BubbleBoardCell() = default;
	BubbleBoardCell(const BubbleBoardCell&) = default;
	BubbleBoardCell(BubbleBoardCell&&) noexcept = default;
	~BubbleBoardCell() = default;

	BubbleBoardCell& operator= (const BubbleBoardCell&) = default;
	BubbleBoardCell& operator= (BubbleBoardCell&&) noexcept = default;

	constexpr bool operator== (const BubbleBoardCell& right) const { return _owner == right._owner && _column == right._column; }
	constexpr std::partial_ordering operator<=> (const BubbleBoardCell& right) const;

public:
	constexpr bool isValid() const { return _owner != nullptr; }

	constexpr ColumnIndex getColumn() const { return _column; }
	constexpr Position getPosition() const { return { getRow(), _column }; }

	constexpr bool isEmpty() const { return !isValid() || _bubble == nullptr; }

	constexpr const std::shared_ptr<Bubble>& getBubble() const { return _bubble; }
	inline void setBubble(const std::shared_ptr<Bubble>& bubble) { clear(), _bubble = bubble; }

	inline Bubble* operator-> () const { return _bubble.get(); }

public:
	constexpr RowIndex getRow() const;

public:
	bool clear();

private:
	inline void build(BubbleBoardRow& row, ColumnIndex column)
	{
		_owner = std::addressof(row);
		_column = column;
		_bubble.reset();
	}
};



class BubbleBoardRow
{
public:
	using Cell = BubbleBoardCell;
	using iterator = typename std::vector<Cell>::iterator;
	using const_iterator = typename std::vector<Cell>::const_iterator;

	friend BubbleBoard;

private:
	Reference<BubbleBoard> _owner = nullptr;
	RowIndex _row = 0;
	bool _little = false;
	std::vector<Cell> _cells;
	std::size_t _bubbleCount;

public:
	BubbleBoardRow() = default;
	BubbleBoardRow(const BubbleBoardRow&) = default;
	BubbleBoardRow(BubbleBoardRow&&) noexcept = default;
	~BubbleBoardRow() = default;

	BubbleBoardRow& operator= (const BubbleBoardRow&) = default;
	BubbleBoardRow& operator= (BubbleBoardRow&&) noexcept = default;

	inline bool operator== (const BubbleBoardRow& right) const { _owner == right._owner && _row == right._row; }
	constexpr std::partial_ordering operator<=> (const BubbleBoardRow& right) const
	{
		if (_owner != right._owner)
			return std::partial_ordering::unordered;
		return _row <=> right._row;
	}

public:
	constexpr bool isValid() const { return _owner != nullptr && !_cells.empty(); }

	constexpr RowIndex getRow() const { return _row; }

	constexpr bool isLittle() const { return _little; }

	constexpr std::size_t getBubblesCount() const { return _bubbleCount; }

	constexpr Reference<Cell> getCell(ColumnIndex column) { return std::addressof(_cells[column]); }
	constexpr ConstReference<Cell> getCell(ColumnIndex column) const { return std::addressof(_cells[column]); }
	
	inline const std::shared_ptr<Bubble>& getBubble(ColumnIndex column) const { return _cells[column].getBubble(); }
	constexpr bool hasBubble(ColumnIndex column) const { return _cells[column].isEmpty(); }

public:
	constexpr ColumnCount getColumns() const { return 1; }

public:
	std::shared_ptr<Bubble> setBubble(ColumnIndex column, const std::shared_ptr<Bubble>& bubble);
	std::shared_ptr<Bubble> removeBubble(ColumnIndex column);

	std::size_t clear();

	void descend();

	bool hasInvalidBottomBubble() const;

public:
	constexpr iterator begin() { return _cells.begin(); }
	constexpr const_iterator begin() const { return _cells.begin(); }
	constexpr const_iterator cbegin() const { return _cells.cbegin(); }
	constexpr iterator end() { return _cells.end(); }
	constexpr const_iterator end() const { return _cells.end(); }
	constexpr const_iterator cend() const { return _cells.cend(); }

	constexpr void forEach(const std::function<void(Cell&)>& action)
	{
		for (Cell& cell : _cells)
			action(cell);
	}

	constexpr void forEach(const std::function<void(const Cell&)>& action) const
	{
		for (const Cell& cell : _cells)
			action(cell);
	}

private:
	constexpr int odd() const { return int(_little) & 0x1; }

private:
	void build(BubbleBoard& board, RowIndex row, BoardColumnStyle columnStyle, bool little);
};

constexpr std::partial_ordering BubbleBoardCell::operator<=> (const BubbleBoardCell& right) const
{
	auto cmp = *_owner <=> *right._owner;
	if (cmp != 0)
		return cmp;
	return _column <=> right._column;
}

constexpr RowIndex BubbleBoardCell::getRow() const { return _owner->getRow(); }



class DescentManager
{
public:
	friend BubbleBoard;

private:
	static constexpr float MaxDelay = 0.1625f;

private:
	struct Row
	{
		std::vector<std::shared_ptr<Bubble>> bubbles;
		bool hide;
	};

private:
	Reference<BubbleBoard> _owner = nullptr;
	std::queue<Row> _rows;
	float _delay = 0;

public:
	DescentManager() = default;
	DescentManager(const DescentManager&) = default;
	DescentManager(DescentManager&&) noexcept = default;
	~DescentManager() = default;

	DescentManager& operator= (const DescentManager&) = default;
	DescentManager& operator= (DescentManager&&) noexcept = default;

public:
};



class HideRow
{
private:
	HideBoardId _boardId;
	RowIndex _row;
	std::vector<MetaBubble> _bubbles;
	mutable Int32 _count = -1;

public:
	HideRow() = default;
	HideRow(const HideRow&) = default;
	HideRow(HideRow&&) noexcept = default;
	~HideRow() = default;

	HideRow& operator= (const HideRow&) = default;
	HideRow& operator= (HideRow&&) noexcept = default;

public:
	constexpr bool isEmpty() const { return _bubbles.empty(); }
	constexpr bool empty() const { return _bubbles.empty(); }

	constexpr std::size_t count() const
	{
		if (_count < 0)
		{
			_count = 0;
			for (const auto& bubble : _bubbles)
				_count++;
		}
		return _count;
	}

	constexpr std::size_t size() const { return count(); }

	constexpr const std::vector<MetaBubble>& getBubbles() const { return _bubbles; }

public:
	std::vector<std::shared_ptr<Bubble>> makeBubblesRow(BubbleGenerator& bgen);

private:
	inline void build(HideBoardId boardId, RowIndex row, const std::vector<MetaBubble>& bubbles)
	{
		_boardId = boardId;
		_row = row;
		_bubbles = bubbles;
		_count = -1;
	}

	inline void build(HideBoardId boardId, RowIndex row, std::vector<MetaBubble>&& bubbles)
	{
		_boardId = boardId;
		_row = row;
		_bubbles = std::move(bubbles);
		_count = -1;
	}

	inline void build(HideBoardId boardId, RowIndex row) { build(boardId, row, {}); }

public:
	friend HideBoard;
};



class HideBoard
{
private:
	static constexpr RowCount MaxRows = utils::level::VisibleRows;
	static constexpr Int32 InvalidCount = -1;

private:
	std::vector<std::unique_ptr<HideRow>> _rows;
	mutable Int32 _count = InvalidCount;

public:
	HideBoard() = default;
	HideBoard(HideBoard&&) noexcept = default;
	~HideBoard() = default;

	HideBoard& operator= (HideBoard&&) noexcept = default;

public:
	constexpr HideBoard(const HideBoard& right) : _rows(right._rows.size()), _count(right._count)
	{
		const std::size_t len = _rows.size();
		for (std::size_t i = 0; i < len; ++i)
			_rows[i].reset(new HideRow(*right._rows[i]));
	}

	constexpr HideBoard& operator= (const HideBoard& right)
	{
		_rows.resize(right._rows.size());
		_count = right._count;

		const std::size_t len = _rows.size();
		for (std::size_t i = 0; i < len; ++i)
			_rows[i].reset(new HideRow(*right._rows[i]));
		return *this;
	}

public:
	constexpr bool isEmpty() const { return _rows.empty(); }
	constexpr bool empty() const { return _rows.empty(); }

	constexpr std::size_t count() const
	{
		if (_count < 0)
		{
			_count = 0;
			for (const auto& row : _rows)
				_count += Int32(row->count());
		}
		return _count;
	}

	constexpr std::size_t size() const { return count(); }

	inline void addRow(std::unique_ptr<HideRow>&& row)
	{
		if (_rows.size() >= MaxRows)
		{
			logger::error("HideBoard rows overflow");
			return;
		}
		if (row == nullptr)
		{
			logger::error("Cannot add to HideBoard a nullptr row");
			return;
		}
		_rows.push_back(std::move(row));
		_count = InvalidCount;
	}

	inline HideRow& getNextRow() { return *_rows.front(); }
	inline const HideRow& getNextRow() const { return *_rows.front(); }

	inline void popRow()
	{
		_rows.erase(_rows.begin());
		_count = InvalidCount;
	}

	inline void trimTop()
	{
		while (!_rows.empty() && _rows.back()->empty())
			_rows.pop_back();
	}

	inline std::vector<std::shared_ptr<Bubble>> buildNextRow(BubbleGenerator& bgen)
	{
		if (!empty())
		{
			auto row = getNextRow().makeBubblesRow(bgen);
			popRow();
			return row;
		}
		return {};
	}

	inline std::vector<std::vector<std::shared_ptr<Bubble>>> buildAllRows(BubbleGenerator& bgen)
	{
		auto rows = std::vector<std::vector<std::shared_ptr<Bubble>>>(_rows.size());
		for (Int64 i = Int64(rows.size()); i >= 0; --i)
			rows[i] = buildNextRow(bgen);
		return rows;
	}

public:
	friend HideBubbleContainer;
};



class HideBubbleContainer
{
private:
	HiddenBubbleContainerType _type;
	BoardColumnStyle _columnStyle;
	std::shared_ptr<BubbleGenerator> _bgen;
	RNG _rand;
	Uint64 _maxBoards = 0;
	std::vector<std::unique_ptr<HideBoard>> _boards;
	std::unique_ptr<HideBoard> _current;

public:
	HideBubbleContainer() = default;
	HideBubbleContainer(const HideBubbleContainer&) = default;
	HideBubbleContainer(HideBubbleContainer&&) noexcept = default;
	~HideBubbleContainer() = default;

	HideBubbleContainer& operator= (const HideBubbleContainer&) = default;
	HideBubbleContainer& operator= (HideBubbleContainer&&) noexcept = default;

public:
	constexpr bool isEmpty() const { return empty(); }
	constexpr bool empty() const { return _boards.empty() && (_current == nullptr || _current->empty()); }

	constexpr bool isDiscrete() const { return _type.discrete; }

public:
	void build(Scenario& scenario, BoardColumnStyle columnStyle);

	void fill(const std::vector<MetaBubbleBoard>& metaBoards);

	std::vector<std::vector<std::shared_ptr<Bubble>>> generate();
	std::vector<std::vector<std::shared_ptr<Bubble>>> generateBoard();
	std::vector<std::shared_ptr<Bubble>> generateRow();

	Uint32 getBubbleCount() const;

private:
	void checkNext();

	std::unique_ptr<HideRow> makeHideRow(const MetaBubbleBoard& board, RowIndex row, HideBoardId boardId);
};



class BubbleBoard
{

};
