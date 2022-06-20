#pragma once

#include "bubbles.h"
#include "level.h"
#include "utils/StaticArray.h"
#include "utils/BasicReference.h"


class Scenario;

class BubbleBoard;
class BubbleBoardRow;





struct BubbleBoardCellPosition
{
	RowIndex row = 0;
	ColumnIndex column = 0;

	constexpr BubbleBoardCellPosition() = default;
	constexpr BubbleBoardCellPosition(const BubbleBoardCellPosition&) = default;
	constexpr BubbleBoardCellPosition(BubbleBoardCellPosition&&) noexcept = default;
	constexpr ~BubbleBoardCellPosition() = default;

	constexpr BubbleBoardCellPosition& operator= (const BubbleBoardCellPosition&) = default;
	constexpr BubbleBoardCellPosition& operator= (BubbleBoardCellPosition&&) noexcept = default;

	constexpr bool operator== (const BubbleBoardCellPosition&) const = default;
	constexpr auto operator<=> (const BubbleBoardCellPosition&) const = default;

	constexpr BubbleBoardCellPosition(RowIndex row, ColumnIndex column = 0) : row(row), column(column) {}
};




class BubbleBoardCell
{
private:
	BubbleBoardRow* _owner = nullptr;
	ColumnIndex _column = 0;
	std::shared_ptr<Bubble> _bubble;

public:
	BubbleBoardCell() = default;
	BubbleBoardCell(const BubbleBoardCell&) = default;
	BubbleBoardCell(BubbleBoardCell&&) noexcept = default;
	~BubbleBoardCell() = default;

public:
	inline bool isValid() const { return _owner; }

	inline ColumnIndex getColumn() const { return _column; }
	inline BubbleBoardCellPosition getPosition() const { return { getRow(), _column }; }

	inline bool isEmpty() const { return isValid() && static_cast<bool>(_bubble); }

	inline bool hasBubble() const { return _bubble != nullptr; }
	inline std::shared_ptr<Bubble> getBubble() const { return _bubble; }
	inline void setBubble(const std::shared_ptr<Bubble>& bubble) { _bubble = bubble; }
	inline void removeBubble() { _bubble.reset(); }

	inline BubbleBoardRow& getBoardRow() { return *_owner; }
	inline const BubbleBoardRow& getBoardRow() const { return *_owner; }

public:
	RowIndex getRow() const;

public:
	inline operator bool() const { return _owner; }
	inline bool operator! () const { return !_owner; }

	inline Bubble* operator-> () const { return _bubble.get(); }

private:
	void create(BubbleBoardRow& board, ColumnIndex column);

public:
	friend BubbleBoardRow;
};






class BubbleBoardRow
{
private:
	BubbleBoard* _owner = nullptr;
	RowIndex _row = 0;
	bool _little = false;
	StaticArray<BubbleBoardCell> _cells;
	Size _bubbleCount = 0;

public:
	BubbleBoardRow() = default;
	BubbleBoardRow(const BubbleBoardRow&) = default;
	BubbleBoardRow(BubbleBoardRow&&) noexcept = default;

	BubbleBoardRow& operator= (const BubbleBoardRow&) = default;
	BubbleBoardRow& operator= (BubbleBoardRow&&) noexcept = default;

	bool operator== (const BubbleBoardRow&) const = default;
	std::strong_ordering operator<=> (const BubbleBoardRow& right) const;

public:
	inline ~BubbleBoardRow() { clear(); }

	inline bool isValid() const { return _owner && _cells; }

	inline ColumnCount size() const { return getColumns(); }

	inline RowIndex getRow() const { return _row; }

	inline bool isLittle() const { return _little; }

	inline Size getBubbleCount() const { return _bubbleCount; }

	inline const BubbleBoardCell& getCell(ColumnIndex column) const { return _cells[column]; }
	inline const ArrayView<const BubbleBoardCell> getCells() const { return _cells; }
	inline std::shared_ptr<Bubble> getBubble(ColumnIndex column) const { return getCell(column).getBubble(); }

	inline bool hasBubble(ColumnIndex column) const { return getBubble(column) != nullptr; }

public:
	int clear();

	ColumnCount getColumns() const;

	void setBubble(ColumnIndex column, const std::shared_ptr<Bubble>& bubble);
	std::shared_ptr<Bubble> removeBubble(ColumnIndex column);

	void descend();

	bool hasInvalidBottomBubble() const;


private:
	void create(BubbleBoard& board, RowIndex row, BoardColumnStyle columns, bool little);

private:
	static bool _unsafeRemoveBubble(BubbleBoardCell& cell);

private:
	inline bool _unsafeRemoveBubble(ColumnIndex column) { return _unsafeRemoveBubble(_cells[column]); }

public:
	using iterator = decltype(_cells)::iterator;
	using const_iterator = decltype(_cells)::const_iterator;

	inline iterator begin() { return _cells.begin(); }
	inline const_iterator begin() const { return _cells.begin(); }
	inline const_iterator cbegin() const { return _cells.cbegin(); }
	inline iterator end() { return _cells.end(); }
	inline const_iterator end() const { return _cells.end(); }
	inline const_iterator cend() const { return _cells.cend(); }

public:
	friend BubbleBoard;
};

inline RowIndex BubbleBoardCell::getRow() const { return isValid() ? _owner->getRow() : 0; }







class DescentManager
{
private:
	static constexpr float maxDelay = 0.1625;

private:
	struct Row
	{
		std::vector<std::shared_ptr<Bubble>> bubbles;
		bool hide;
	};

private:
	BubbleBoard* _owner;
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








class BubbleBoard
{
private:
	BoardColumnStyle _columns;


public:

public:
	inline BoardColumnStyle getColumns() const { return _columns; }

public:
};
