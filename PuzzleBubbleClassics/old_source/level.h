#pragma once

#include "bubbles.h"

import Utils.Arrays;

typedef UInt32 RowCount;
typedef UInt32 RowIndex;

typedef UInt32 ColumnCount;
typedef UInt32 ColumnIndex;

namespace levels
{
	static constexpr ColumnCount MinColumnCount = 8;
	static constexpr ColumnCount MaxColumnCount = 16;

	static constexpr RowCount VisibleRows = 14;
	static constexpr RowCount BottomRows = 3;
	static constexpr RowCount TopRows = 1;
	static constexpr RowCount HiddenRows = (BottomRows + TopRows);
	static constexpr RowCount TotalRows = VisibleRows + HiddenRows;
}


enum class BoardColumnStyle : ColumnCount
{
	Min = 8,
	VerySmall = 9,
	Small = 10,
	SmallMedium = 11,
	Medium = 12,
	LargeMedium = 13,
	Large = 14,
	VeryLarge = 15,
	Max = 16
};


namespace levels
{
	constexpr ColumnCount validateStyleAsColumns(ColumnCount columns)
	{
		using enum BoardColumnStyle;
		return utils::clamp(columns, static_cast<ColumnCount>(Min), static_cast<ColumnCount>(Max));
	}

	constexpr BoardColumnStyle validateStyle(BoardColumnStyle style)
	{
		using enum BoardColumnStyle;
		return static_cast<BoardColumnStyle>(validateStyleAsColumns(static_cast<ColumnCount>(style)));
	}

	constexpr ColumnCount styleToColumns(BoardColumnStyle style)
	{
		return static_cast<ColumnCount>(validateStyle(style));
	}

	constexpr BoardColumnStyle columnsToStyle(ColumnCount columns)
	{
		return validateStyle(static_cast<BoardColumnStyle>(columns));
	}

	constexpr bool isPairRow(RowIndex row) { return utils::is_pair(row); }

	constexpr ColumnCount adaptIfIsOdd(RowIndex row, ColumnCount columns)
	{
		return isPairRow(row) ? columns : columns - 1;
	}

	constexpr ColumnCount adaptIfIsOdd(RowIndex row, BoardColumnStyle style)
	{
		return adaptIfIsOdd(row, styleToColumns(style));
	}
}


enum class HiddenBubbleContainerType : UInt8
{
	Discrete = 0x1,
	Random = 0x2,
	Endless = 0x4,

	Continuous = 0x0,
	Sequential = 0x0,
	Finite = 0x0,

	FiniteSequentialContinuous = Finite | Sequential | Continuous,
	FiniteSequentialDiscrete = Finite | Sequential | Discrete,
	FiniteRandomContinuous = Finite | Random | Continuous,
	FiniteRandomDiscrete = Finite | Random | Discrete,
	EndlessSequentialContinuous = Endless | Sequential | Continuous,
	EndlessSequentialDiscrete = Endless | Sequential | Discrete,
	EndlessRandomContinuous = Endless | Random | Continuous,
	EndlessRandomDiscrete = Endless | Random | Discrete,
};

constexpr HiddenBubbleContainerType operator| (HiddenBubbleContainerType left, HiddenBubbleContainerType right)
{
	return static_cast<HiddenBubbleContainerType>(static_cast<UInt8>(left) | static_cast<UInt8>(right));
}

constexpr HiddenBubbleContainerType operator& (HiddenBubbleContainerType left, HiddenBubbleContainerType right)
{
	return static_cast<HiddenBubbleContainerType>(static_cast<UInt8>(left) & static_cast<UInt8>(right));
}


namespace levels
{
	constexpr bool hasType(HiddenBubbleContainerType containerType, HiddenBubbleContainerType typeToSearch)
	{
		return (containerType & typeToSearch) == typeToSearch;
	}

	constexpr bool isDiscrete(HiddenBubbleContainerType containerType) { return hasType(containerType, HiddenBubbleContainerType::Discrete); }
	constexpr bool isContiguous(HiddenBubbleContainerType containerType) { return !hasType(containerType, HiddenBubbleContainerType::Discrete); }

	constexpr bool isRandom(HiddenBubbleContainerType containerType) { return hasType(containerType, HiddenBubbleContainerType::Random); }
	constexpr bool isSequential(HiddenBubbleContainerType containerType) { return !hasType(containerType, HiddenBubbleContainerType::Random); }

	constexpr bool isEndless(HiddenBubbleContainerType containerType) { return hasType(containerType, HiddenBubbleContainerType::Endless); }
	constexpr bool isFinite(HiddenBubbleContainerType containerType) { return !hasType(containerType, HiddenBubbleContainerType::Endless); }
}






class MetaGoals
{
private:
	std::map<BubbleIdentifier, UInt32> _bubgoals;
	UInt32 _timesToClearBoard = 0;

public:
	MetaGoals() = default;
	MetaGoals(const MetaGoals&) = default;
	MetaGoals(MetaGoals&&) noexcept = default;
	~MetaGoals() = default;

	MetaGoals& operator= (const MetaGoals&) = default;
	MetaGoals& operator= (MetaGoals&&) noexcept = default;

public:
	UInt32 getBubbleGoals(const BubbleIdentifier& bubbleId) const;
	void setBubbleGoals(const BubbleIdentifier& bubbleId, UInt32 timesToDestroy);

	void forEachBubbleGoals(const std::function<void(const BubbleIdentifier&, UInt32)>& action);

public:
	inline UInt32 getTimesToClearBoard() const { return _timesToClearBoard; }
	inline void setTimesToClearBoard(UInt32 times) { _timesToClearBoard = times; }

	inline const std::map<BubbleIdentifier, UInt32>& getAllBubbleGoals() const { return _bubgoals; }
	inline void setAllBubbleGoals(const std::map<BubbleIdentifier, UInt32>& goals) { _bubgoals = goals; }
	inline void setAllBubbleGoals(std::map<BubbleIdentifier, UInt32>&& goals) { _bubgoals = std::move(goals); }
	inline void removeBubbleGoals(const BubbleIdentifier& bubbleId) { setBubbleGoals(bubbleId, 0); }
	inline UInt32 getBubbleGoals(const String& modelName, BubbleColor color) { return getBubbleGoals({ modelName, color }); }
};






class BinaryBubbleBoard
{
private:
	BubbleIdentifier* _rows = nullptr;
	BoardColumnStyle _columns = BoardColumnStyle::Min;

public:
	BinaryBubbleBoard() = default;

	inline BinaryBubbleBoard(const BinaryBubbleBoard& other) : BinaryBubbleBoard() { _copy(other, false); }
	inline BinaryBubbleBoard(BinaryBubbleBoard&& other) noexcept : BinaryBubbleBoard() { _move(std::move(other), false); }
	inline ~BinaryBubbleBoard() { _destroy(); }

	inline BinaryBubbleBoard& operator= (const BinaryBubbleBoard& right) { return _copy(right, true); }
	inline BinaryBubbleBoard& operator= (BinaryBubbleBoard&& right) noexcept { return _move(std::move(right), true); }

public:
	BinaryBubbleBoard(BoardColumnStyle columns);

	void setColumnStyle(BoardColumnStyle columns);

	BubbleIdentifier& insertBubble(RowIndex row, ColumnIndex column, const BubbleIdentifier& bubble);
	BubbleIdentifier& insertBubble(RowIndex row, ColumnIndex column, const String& modelName, BubbleColor color);

	ArrayView<BubbleIdentifier> getRow(RowIndex row);
	const ArrayView<const BubbleIdentifier> getRow(RowIndex row) const;

public:
	inline BoardColumnStyle getColumnStyle() const { return _columns; }

	inline BubbleIdentifier& getBubble(RowIndex row, ColumnIndex column) { return _bubble(row, column); }
	inline const BubbleIdentifier& getBubble(RowIndex row, ColumnIndex column) const { return _bubble(row, column); }

	inline BubbleIdentifier& operator[] (const std::pair< RowIndex, ColumnIndex>& position) { return _bubble(position.first, position.second); }
	inline const BubbleIdentifier& operator[] (const std::pair< RowIndex, ColumnIndex>& position) const { return _bubble(position.first, position.second); }

private:
	void _destroy();
	BinaryBubbleBoard& _copy(const BinaryBubbleBoard& other, bool reset);
	BinaryBubbleBoard& _move(BinaryBubbleBoard&& other, bool reset) noexcept;
	void _swap(BinaryBubbleBoard& other);

private:
	static constexpr RowCount _rowCount() { return levels::VisibleRows; }
	static constexpr ColumnCount _columnCount(BoardColumnStyle columns) { return levels::styleToColumns(columns); }
	constexpr ColumnCount _columnCount() const { return _columnCount(_columns); }

	static constexpr Size _rowIdx(RowIndex row) { return utils::clamp(row, 0, _rowCount()); }
	constexpr Size _columnIdx(ColumnIndex column) const { return utils::clamp(column, 0, _columnCount()); }

	constexpr Size _rowColumnToIdx(RowIndex row, ColumnIndex column) const { return _rowIdx(row) * static_cast<Size>(_columnCount()) + _columnIdx(column); }

	constexpr Size _bubsCount() const { return static_cast<Size>(_rowCount()) * static_cast<Size>(_columnCount()); }
	static constexpr Size _bubsCount(BoardColumnStyle columns) { return static_cast<Size>(_rowCount()) * static_cast<Size>(_columnCount(columns)); }

	constexpr BubbleIdentifier& _bubble(RowIndex row, ColumnIndex column) { return _rows[_rowColumnToIdx(row, column)]; }
	constexpr const BubbleIdentifier& _bubble(RowIndex row, ColumnIndex column) const { return _rows[_rowColumnToIdx(row, column)]; }
};








class LevelProperties
{
private:
	BoardColumnStyle _columns = BoardColumnStyle::Min;
	PlayerId _playerid = PlayerId::Single;
	std::vector<BinaryBubbleBoard> _bubbles;
	HiddenBubbleContainerType _hideType = HiddenBubbleContainerType::Continuous;
	UInt32 _clearBoardsRequired = 0U;
	BubbleColorMask _availableColors = 0xFFU;
	RNG::Seed _seed = 0;
	UInt32 _initialBubbles = 0;
	bool _generateUpBubbles = false;
	RandomBubbleModelSelector _arrowBubbleSelector;
	RandomBubbleModelSelector _boardBubbleSelector;
	bool _roof = false;
	bool _remote = false;
	bool _enableTimer = true;
	bool _hideTimer = true;
	UInt32 _timerTurnTime = 10;
	UInt32 _timerEndTime = 90;
	TimerMode _timerMode = TimerMode::TURN;
	bool _enableBubbleSwap = true;
	std::string _background;
	std::string _music;
	MetaGoals _goals;

	RNG _rand;
	bool _randReady = false;

public:
	LevelProperties() = default;
	LevelProperties(const LevelProperties&) = default;
	LevelProperties(LevelProperties&&) = default;
	~LevelProperties() = default;

	LevelProperties& operator= (const LevelProperties&) = default;
	LevelProperties& operator= (LevelProperties&&) = default;

	BoardColumnStyle getColuns() const;
	void setColuns(BoardColumnStyle columns);

	PlayerId getPlayer() const;
	void setPlayer(PlayerId player);

	UInt32 getBubbleBoardCount() const;
	void setBubbleBoardCount(UInt32 count);

	const BinaryBubbleBoard& getBubbleBoard(UInt32 index) const;
	BinaryBubbleBoard& peekBubbleBoard(UInt32 index);

	HiddenBubbleContainerType getHiddenBubbleContainerType() const;
	void setHiddenBubbleContainerType(HiddenBubbleContainerType type);

	UInt32 getClearedBoardRequiredCount() const;
	void setClearedBoardRequiredCount(UInt32 amount);

	BubbleColorMask getEnabledColors() const;
	bool isColorEnabled(const BubbleColor& color) const;
	void setColorEnabled(const BubbleColor& color, bool enabled);

	RNG::Seed getSeed() const;
	bool isRandomSeed() const;
	void setSeed(RNG::Seed seed);
	void setSeedRandom();
	RNG generateRNG();

	UInt32 getInitialFilledRows() const;
	void setInitialFilledRows(UInt32 count);

	bool isBubbleGenerationEnabled() const;
	void setBubbleGenerationEnabled(bool enabled);

	const RandomBubbleModelSelector& getArrowModelSelector() const;
	const RandomBubbleModelSelector& getBoardModelSelector() const;
	RandomBubbleModelSelector& peekArrowModelSelector();
	RandomBubbleModelSelector& peekBoardModelSelector();

	bool isRoofEnabled() const;
	void setRoofEnabled(bool enabled);

	bool isRemoteBubblesEnabled() const;
	void setRemoteBubblesEnabled(bool enabled);

	bool isHideTimer() const;
	void setHideTimer(bool enabled);

	UInt32 getTimerTurnTime() const;
	void setTimerTurnTime(UInt32 seconds);

	UInt32 getTimerEndTime() const;
	void setTimerEndTime(UInt32 seconds);

	TimerMode getTimerMode() const;
	void setTimerMode(TimerMode mode);

	bool isBubbleSwapEnabled() const;
	void setBubbleSwapEnabled(bool enabled);

	const std::string& getBackground() const;
	void setBackground(const std::string& textureName);

	const MetaGoals& getGoals() const;
	MetaGoals& peekGoals();
};
