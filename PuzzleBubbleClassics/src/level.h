#pragma once

#include "bubble.h"

#include "utils/math.h"


using RowCount = Uint32;
using RowIndex = Uint32;

using ColumnCount = Uint32;
using ColumnIndex = Uint32;


namespace utils::level
{
	static constexpr ColumnCount MinColumnCount = 8;
	static constexpr ColumnCount MaxColumnCount = 16;

	static constexpr RowCount VisibleRows = 14;
	static constexpr RowCount BottomRows = 3;
	static constexpr RowCount TopRows = 1;
	static constexpr RowCount HiddenRows = (BottomRows + TopRows);
	static constexpr RowCount TotalRows = VisibleRows + HiddenRows;
}


enum class PlayerId : Uint8
{
	First = 0,
	Second = 1,

	Single = First
};


enum class TimerMode : Uint8
{
	None = 0b00,
	Turn = 0b01,
	End = 0b10,
	TurnAndEnd = Turn | End
};


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

namespace utils::level
{
	constexpr ColumnCount validateColumnStyle(ColumnCount columns) noexcept
	{
		return utils::clamp(columns, ColumnCount(BoardColumnStyle::Min), ColumnCount(BoardColumnStyle::Max));
	}

	constexpr BoardColumnStyle validateColumnStyle(BoardColumnStyle style) noexcept
	{
		return BoardColumnStyle(validateColumnStyle(ColumnCount(style)));
	}

	constexpr ColumnCount columnStyleToColumns(BoardColumnStyle style) noexcept { return ColumnCount(validateColumnStyle(style)); }

	constexpr BoardColumnStyle columnsToColumnStyle(ColumnCount columns) noexcept { return BoardColumnStyle(validateColumnStyle(columns)); }

	constexpr bool isPairRow(RowIndex row) { return utils::isPair(row); }

	constexpr bool isOddRow(RowIndex row) { return utils::isOdd(row); }

	constexpr ColumnCount adaptColumnCountIfRowIsOdd(RowIndex row, ColumnCount columns)
	{
		return isPairRow(row) ? columns : columns - 1;
	}

	constexpr ColumnCount adaptColumnCountIfRowIsOdd(RowIndex row, BoardColumnStyle style)
	{
		return adaptColumnCountIfRowIsOdd(row, columnStyleToColumns(style));
	}

	constexpr std::size_t maxNumBubblesOnBoard(ColumnCount numColumns, RowCount numRows = VisibleRows) noexcept
	{
		return std::size_t(numColumns) * std::size_t(numRows) - std::size_t(numRows / 2);
	}

	constexpr std::size_t maxNumBubblesOnBoard(BoardColumnStyle style, RowCount numRows = VisibleRows) noexcept
	{
		return maxNumBubblesOnBoard(columnStyleToColumns(style), numRows);
	}
}


enum class HiddenBubbleContainerMode : Uint8
{
	Discrete = 0b001,
	Random   = 0b010,
	Endless  = 0b100,

	FiniteSequentialContinuous = 0,
	FiniteSequentialDiscrete = Discrete,
	FiniteRandomContinuous = Random,
	FiniteRandomDiscrete = Random | Discrete,
	EndlessSequentialContinuous = Endless,
	EndlessSequentialDiscrete = Endless | Discrete,
	EndlessRandomContinuous = Endless | Random,
	EndlessRandomDiscrete = Endless | Random | Discrete,

	Default = FiniteSequentialContinuous
};

static constexpr HiddenBubbleContainerMode operator| (HiddenBubbleContainerMode left, HiddenBubbleContainerMode right) noexcept
{
	return HiddenBubbleContainerMode(Uint8(left) | Uint8(right));
}

static constexpr HiddenBubbleContainerMode operator& (HiddenBubbleContainerMode left, HiddenBubbleContainerMode right) noexcept
{
	return HiddenBubbleContainerMode(Uint8(left) & Uint8(right));
}

static constexpr HiddenBubbleContainerMode operator~ (HiddenBubbleContainerMode right) noexcept
{
	return HiddenBubbleContainerMode(~Uint8(right));
}

static constexpr HiddenBubbleContainerMode& operator|= (HiddenBubbleContainerMode& left, HiddenBubbleContainerMode right) noexcept
{
	return left = HiddenBubbleContainerMode(Uint8(left) | Uint8(right)), left;
}

static constexpr HiddenBubbleContainerMode& operator&= (HiddenBubbleContainerMode& left, HiddenBubbleContainerMode right) noexcept
{
	return left = HiddenBubbleContainerMode(Uint8(left) & Uint8(right)), left;
}


struct HiddenBubbleContainerType
{
	bool discrete : 1 = false;
	bool random : 1 = false;
	bool endless : 1 = false;

public:
	constexpr HiddenBubbleContainerType() noexcept = default;
	constexpr HiddenBubbleContainerType(const HiddenBubbleContainerType&) noexcept = default;
	constexpr HiddenBubbleContainerType(HiddenBubbleContainerType&&) noexcept = default;
	constexpr ~HiddenBubbleContainerType() noexcept = default;

	constexpr HiddenBubbleContainerType& operator= (const HiddenBubbleContainerType&) noexcept = default;
	constexpr HiddenBubbleContainerType& operator= (HiddenBubbleContainerType&&) noexcept = default;

	constexpr bool operator== (const HiddenBubbleContainerType&) const noexcept = default;

public:
	constexpr HiddenBubbleContainerType(bool discrete, bool random = false, bool endless = false) noexcept : discrete(discrete), random(random), endless(endless) {}
	constexpr explicit HiddenBubbleContainerType(HiddenBubbleContainerMode mode) :
		discrete((mode & HiddenBubbleContainerMode::Discrete) == HiddenBubbleContainerMode::Discrete),
		random((mode & HiddenBubbleContainerMode::Random) == HiddenBubbleContainerMode::Random),
		endless((mode & HiddenBubbleContainerMode::Endless) == HiddenBubbleContainerMode::Endless)
	{}

	constexpr explicit operator HiddenBubbleContainerMode() const noexcept
	{
		HiddenBubbleContainerMode mode = HiddenBubbleContainerMode(0);
		if (discrete) mode |= HiddenBubbleContainerMode::Discrete;
		if (random) mode |= HiddenBubbleContainerMode::Random;
		if (endless) mode |= HiddenBubbleContainerMode::Endless;
		return mode;
	}

public:
	constexpr bool isDiscrete() const { return discrete; }
	constexpr bool isContinuous() const { return !discrete; }

	constexpr bool isRandom() const { return random; }
	constexpr bool isSequential() const { return !random; }

	constexpr bool isEndless() const { return endless; }
	constexpr bool isFinite() const { return !endless; }
};



class MetaGoals
{
public:
	using BubbleGoalsMap = std::unordered_map<MetaBubble, Uint32>;

private:
	BubbleGoalsMap _bubbleGoals;
	Uint32 _timesToClearBoard = 0;

public:
	MetaGoals() = default;
	MetaGoals(const MetaGoals&) = default;
	MetaGoals(MetaGoals&&) noexcept = default;
	~MetaGoals() = default;

	MetaGoals& operator= (const MetaGoals&) = default;
	MetaGoals& operator= (MetaGoals&&) noexcept = default;

public:
	Uint32 getBubbleGoals(const MetaBubble& bubble);
	void setBubbleGoals(const MetaBubble& bubble, Uint32 timesToDestroy);

public:
	constexpr Uint32 getTimesToClearBoard() const { return _timesToClearBoard; }
	constexpr void setTimesToClearBoard(Uint32 times) { _timesToClearBoard = times; }

	constexpr const BubbleGoalsMap& getAllBubbleGoals() const { return _bubbleGoals; }
	inline void setAllBubbleGoals(const BubbleGoalsMap& goals) { _bubbleGoals = goals; }
	inline void setAllBubbleGoals(BubbleGoalsMap&& goals) noexcept { _bubbleGoals = std::move(goals); }
	inline void removeBubbleGoals(const MetaBubble& bubble) { setBubbleGoals(bubble, 0); }

	inline void forEachBubbleGoals(const std::function<void(const MetaBubble&, Uint32)> & action)
	{
		for (const auto& goal : _bubbleGoals)
			action(goal.first, goal.second);
	}
};



class MetaBubbleBoard
{
public:
	using Row = std::vector<MetaBubble>;
	using Board = std::vector<Row>;

private:
	Board _board;
	BoardColumnStyle _columnStyle = BoardColumnStyle::Min;

public:
	MetaBubbleBoard() : MetaBubbleBoard(BoardColumnStyle::Min) {}
	MetaBubbleBoard(const MetaBubbleBoard&) = default;
	MetaBubbleBoard(MetaBubbleBoard&&) noexcept = default;
	~MetaBubbleBoard() = default;

	MetaBubbleBoard& operator= (const MetaBubbleBoard&) = default;
	MetaBubbleBoard& operator= (MetaBubbleBoard&&) noexcept = default;

public:
	explicit MetaBubbleBoard(BoardColumnStyle columnStyle);

	void setColumnStyle(BoardColumnStyle columnStyle);

	bool insertBubble(RowIndex row, ColumnIndex column, const MetaBubble& bubble);

public:
	inline Row& getRow(RowIndex row) { return _board[row]; }
	inline const Row& getRow(RowIndex row) const { return _board[row]; }

public:
	constexpr RowCount getRowsCount() const { return rowsCount(); }
	constexpr ColumnCount getColumnsCount() const { return columnsCount(_columnStyle); }

private:
	static constexpr RowCount rowsCount() noexcept { return utils::level::VisibleRows; }
	static constexpr ColumnCount columnsCount(BoardColumnStyle style) noexcept { return utils::level::columnStyleToColumns(style); }

	static constexpr std::size_t rowIndex(RowIndex row)
	{
#ifdef _DEBUG
		if (row >= rowsCount())
			logger::error("MetaBubbleBoard RowIndex {} out of bounds [0, {}].", row, (rowsCount() - 1));
#endif
		return std::size_t(row);
	}

	constexpr std::size_t columnIndex(ColumnIndex column) const
	{
#ifdef _DEBUG
		if (column >= getColumnsCount())
			logger::error("MetaBubbleBoard ColumnIndex {} out of bounds [0, {}].", column, (getColumnsCount() - 1));
#endif
		return std::size_t(column);
	}

	constexpr std::size_t rowColumnToIndex(RowIndex row, ColumnIndex column) const
	{
		return rowIndex(row) * getColumnsCount() + columnIndex(column);
	}
};



namespace utils::level
{
	using RandomBubbleModelSelectorScoreType = Uint16;

	static constexpr RandomBubbleModelSelectorScoreType RandomBubbleModelSelectorMaxScore = 0xffffU;
	static constexpr RandomBubbleModelSelectorScoreType RandomBubbleModelSelectorMinScore = 0;
}

class RandomBubbleModelSelector;

class RandomBubbleModelSelectorScores
{
public:
	using ScoreType = utils::level::RandomBubbleModelSelectorScoreType;

	friend class RandomBubbleModelSelector;

public:
	static constexpr ScoreType MaxScore = utils::level::RandomBubbleModelSelectorMaxScore;
	static constexpr ScoreType MinScore = utils::level::RandomBubbleModelSelectorMinScore;

private:
	std::unordered_map<std::string, ScoreType> _models;

public:
	RandomBubbleModelSelectorScores() = default;
	RandomBubbleModelSelectorScores(const RandomBubbleModelSelectorScores&) = default;
	RandomBubbleModelSelectorScores(RandomBubbleModelSelectorScores&&) = default;
	~RandomBubbleModelSelectorScores() = default;

	RandomBubbleModelSelectorScores& operator= (const RandomBubbleModelSelectorScores&) = default;
	RandomBubbleModelSelectorScores& operator= (RandomBubbleModelSelectorScores&&) = default;

public:
	void setModelScore(const std::shared_ptr<BubbleModel>& model, ScoreType score);
	ScoreType getModelScore(const std::shared_ptr<BubbleModel>& model) const;

public:
	inline void setModelScore(const std::string& modelName, ScoreType score)
	{
		setModelScore(BubbleModelManager::instance().get(modelName), score);
	}

	inline ScoreType getModelScore(const std::string& modelName) const
	{
		return getModelScore(BubbleModelManager::instance().get(modelName));
	}

	inline void forEach(const std::function<void(const std::string&, ScoreType)>& action) const
	{
		for (const auto& entry : _models)
			action(entry.first, entry.second);
	}
};



union AvailableColorsTable
{
public:
	struct
	{
		bool red : 1;
		bool orange : 1;
		bool yellow : 1;
		bool green : 1;
		bool blue : 1;
		bool purple : 1;
		bool gray : 1;
		bool black : 1;
	};

private:
	Uint8 bits;

public:
	constexpr AvailableColorsTable() noexcept : bits(0) {}
	constexpr AvailableColorsTable(const AvailableColorsTable& right) noexcept : bits(right.bits) {}
	constexpr AvailableColorsTable(AvailableColorsTable&& right) noexcept : bits(right.bits) {}
	constexpr ~AvailableColorsTable() noexcept = default;

	constexpr AvailableColorsTable& operator= (const AvailableColorsTable& right) noexcept { return bits = right.bits, *this; }
	constexpr AvailableColorsTable& operator= (AvailableColorsTable&& right) noexcept { return bits = right.bits, *this; }

	constexpr bool operator== (const AvailableColorsTable& right) const noexcept { return bits == right.bits; }

public:
	constexpr AvailableColorsTable(bool enableAll) noexcept : bits(enableAll ? 0xffu : 0) {}

	constexpr void enable(BubbleColor color) noexcept
	{
		if (color.isNormalColor())
			bits |= 0b1 << (Uint8(color.code()) - 1);
	}

	constexpr void disable(BubbleColor color) noexcept
	{
		if (color.isNormalColor())
			bits &= ~(0b1 << (Uint8(color.code()) - 1));
	}

	constexpr void setEnabled(BubbleColor color, bool enabled) noexcept
	{
		if (enabled)
			enable(color);
		else
			disable(color);
	}

	constexpr bool isEnabled(BubbleColor color) const noexcept
	{
		return color.isNormalColor() && (bits & (0b1 << (Uint8(color.code()) - 1))) != 0;
	}

	constexpr void enableAll() noexcept { bits = 0xffu; }

	constexpr void disableAll() noexcept { bits = 0; }
};



class LevelProperties
{
private:
	BoardColumnStyle _columnStyle = BoardColumnStyle::Min;
	PlayerId _playerId = PlayerId::First;
	std::vector<MetaBubbleBoard> _bubbles;
	HiddenBubbleContainerType _hideType;
	Uint32 _clearBoardsRequired = 0;
	AvailableColorsTable _availableColors = true;
	RNG::SeedType _seed = 0;
	Uint32 _initialBubbles = 0;
	bool _generateUpBubbles = false;
	RandomBubbleModelSelectorScores _arrowBubbleSelector;
	RandomBubbleModelSelectorScores _boardBubbleSelector;
	bool _roof = false;
	bool _remote = false;
	bool _enableTimer = true;
	bool _hideTimer = true;
	Uint32 _timerTurnTime = 10;
	Uint32 _timerEndTime = 90;
	TimerMode _timerMode = TimerMode::Turn;
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

public:
	constexpr BoardColumnStyle getColumnStyle() const { return _columnStyle; }
	constexpr void setColumnStyle(BoardColumnStyle style) { _columnStyle = style; }

	constexpr PlayerId getPlayerId() const { return _playerId; }
	constexpr void setPlayerId(PlayerId playerId) { _playerId = playerId; }

	constexpr std::size_t getBubbleBoardCount() const { return _bubbles.size(); }
	inline void setBubbleBoardCount(std::size_t count)
	{
		_bubbles.clear();
		_bubbles.resize(count);
	}

	constexpr MetaBubbleBoard& getBubbleBoard(std::size_t index) { return _bubbles[index]; }
	constexpr const MetaBubbleBoard& getBubbleBoard(std::size_t index) const { return _bubbles[index]; }

	constexpr HiddenBubbleContainerType getHiddenBubbleContainerType() const { return _hideType; }
	constexpr void setHiddenBubbleContainerType(HiddenBubbleContainerType type) { _hideType = type; }

	constexpr Uint32 getClearBoardRequiredCount() const { return _clearBoardsRequired; }
	constexpr void setClearBoardRequiredCount(Uint32 count) { _clearBoardsRequired = count; }

	constexpr AvailableColorsTable getEnabledColors() const { return _availableColors; }
	constexpr bool isColorEnabled(BubbleColor color) const { return _availableColors.isEnabled(color); }
	constexpr void setColorEnabled(BubbleColor color, bool enabled) { _availableColors.setEnabled(color, enabled); }

	constexpr RNG::SeedType getSeed() const { return _seed; }
	constexpr bool isRandomSeed() const { return _seed == 0; }
	constexpr void setSeed(RNG::SeedType seed) { _seed = seed, _randReady = false; }
	constexpr void setSeedRandom() { _seed = 0, _randReady = false; }
	inline RNG generateRNG()
	{
		if (!_randReady)
		{
			if (_seed != 0)
				_rand = RNG(_seed);
			else
				_rand = RNG();
			_randReady = true;
		}
		return _rand.randomRNG();
	}

	constexpr Uint32 getInitialFilledRows() const { return _initialBubbles; }
	constexpr void setInitialFilledRows(Uint32 count) { _initialBubbles = count; }

	constexpr bool isBubbleGenerationEnabled() const { return _generateUpBubbles; }
	constexpr void setBubbleGenerationEnabled(bool enabled) { _generateUpBubbles = enabled; }

	constexpr RandomBubbleModelSelectorScores& getArrowModelSelectorScores() { return _arrowBubbleSelector; }
	constexpr RandomBubbleModelSelectorScores& getBoardModelSelector() { return _boardBubbleSelector; }
	constexpr const RandomBubbleModelSelectorScores& getArrowModelSelectorScores() const { return _arrowBubbleSelector; }
	constexpr const RandomBubbleModelSelectorScores& getBoardModelSelector() const { return _boardBubbleSelector; }

	constexpr bool isRoofEnabled() const { return _roof; }
	constexpr void setRoofEnabled(bool enabled) { _roof = enabled; }

	constexpr bool isRemoteBubblesEnabled() const { return _remote; }
	constexpr void setRemoteBubblesEnabled(bool enabled) { _remote = enabled; }

	constexpr bool isHideTimer() const { return _hideTimer; }
	constexpr void setHideTimer(bool enabled) { _hideTimer = enabled; }

	constexpr Uint32 getTimerTurnTime() const { return _timerTurnTime; }
	constexpr void setTimerTurnTime(Uint32 seconds) { _timerTurnTime = seconds; }

	constexpr Uint32 getTimerEndTime() const { return _timerEndTime; }
	constexpr void setTimerEndTime(Uint32 seconds) { _timerEndTime = seconds; }

	constexpr TimerMode getTimerMode() const { return _timerMode; }
	constexpr void setTimerMode(TimerMode mode) { _timerMode = mode; }

	constexpr bool isBubbleSwapEnabled() const { return _enableBubbleSwap; }
	constexpr void setBubbleSwapEnabled(bool enabled) { _enableBubbleSwap = enabled; }

	constexpr const std::string& getBackground() const { return _background; }
	constexpr void setBackground(std::string_view background) { _background = background; }

	constexpr MetaGoals& getGoals() { return _goals; }
	constexpr const MetaGoals& getGoals() const { return _goals; }
};
