#include "level.h"


UInt32 MetaGoals::getBubbleGoals(const BubbleIdentifier& bubbleId) const
{
	auto it = _bubgoals.find(bubbleId);
	if (it != _bubgoals.end())
		return it->second;
	return 0;
}

void MetaGoals::setBubbleGoals(const BubbleIdentifier& bubbleId, UInt32 timesToDestroy)
{
	if (timesToDestroy == 0)
		_bubgoals.erase(bubbleId);
	else
		_bubgoals.insert({ bubbleId, timesToDestroy });
}

void MetaGoals::forEachBubbleGoals(const std::function<void(const BubbleIdentifier&, UInt32)>& action)
{
	for (const auto& entry : _bubgoals)
		action(entry.first, entry.second);
}








void BinaryBubbleBoard::_destroy()
{
	if (_rows)
	{
		delete[] _rows;
		_rows = nullptr;
	}
}

BinaryBubbleBoard& BinaryBubbleBoard::_copy(const BinaryBubbleBoard& other, bool reset)
{
	if (reset)
		_destroy();

	if (other._rows)
	{
		utils::construct(*this, other._columns);
		Size len = _bubsCount();
		for (Size i = 0; i < len; ++i)
			_rows[i] = other._rows[i];
	}

	return *this;
}

BinaryBubbleBoard& BinaryBubbleBoard::_move(BinaryBubbleBoard&& other, bool reset) noexcept
{
	if (reset)
		_destroy();

	_rows = other._rows;
	_columns = other._columns;

	other._rows = nullptr;

	return *this;
}

void BinaryBubbleBoard::_swap(BinaryBubbleBoard& other)
{
	auto auxRows = _rows;
	auto auxColumns = _columns;

	_rows = other._rows;
	_columns = other._columns;
	other._rows = auxRows;
	other._columns = auxColumns;
}

BinaryBubbleBoard::BinaryBubbleBoard(BoardColumnStyle columns) :
	_columns(columns),
	_rows(new BubbleIdentifier[_bubsCount(columns)])
{}

void BinaryBubbleBoard::setColumnStyle(BoardColumnStyle columns)
{
	if (columns != _columns)
	{
		if (_rows)
		{
			BinaryBubbleBoard aux(columns);
			_swap(aux);

			const Size len = levels::styleToColumns(std::min(_columns, aux._columns));
			for (Size i = 0; i < len; ++i)
				_rows[i] = std::move(aux._rows[i]);
		}
		else
			utils::construct(*this, columns);
	}
}

BubbleIdentifier& BinaryBubbleBoard::insertBubble(RowIndex row, ColumnIndex column, const BubbleIdentifier& bubble)
{
	return _bubble(row, column) = bubble;
}

BubbleIdentifier& BinaryBubbleBoard::insertBubble(RowIndex row, ColumnIndex column, const String& modelName, BubbleColor color)
{
	return _bubble(row, column) = { modelName, color };
}

ArrayView<BubbleIdentifier> BinaryBubbleBoard::getRow(RowIndex row)
{
	return { std::addressof(_bubble(row, 0)), static_cast<Size>(_columnCount()) };
}

const ArrayView<const BubbleIdentifier> BinaryBubbleBoard::getRow(RowIndex row) const
{
	return { std::addressof(_bubble(row, 0)), static_cast<Size>(_columnCount()) };
}









BoardColumnStyle LevelProperties::getColuns() const { return _columns; }
void LevelProperties::setColuns(BoardColumnStyle columns)
{
	_columns = columns;
	for (auto& bubs : _bubbles)
		bubs.setColumnStyle(columns);
}

PlayerId LevelProperties::getPlayer() const { return _playerid; }
void LevelProperties::setPlayer(PlayerId player) { _playerid = player; }

UInt32 LevelProperties::getBubbleBoardCount() const { return static_cast<UInt32>(_bubbles.size()); }
void LevelProperties::setBubbleBoardCount(UInt32 count)
{
	_bubbles.clear();
	_bubbles.resize(static_cast<size_t>(count), { _columns });
}

const BinaryBubbleBoard& LevelProperties::getBubbleBoard(UInt32 index) const { return _bubbles[static_cast<size_t>(index)]; }
BinaryBubbleBoard& LevelProperties::peekBubbleBoard(UInt32 index) { return _bubbles[static_cast<size_t>(index)]; }

HiddenBubbleContainerType LevelProperties::getHiddenBubbleContainerType() const { return _hideType; }
void LevelProperties::setHiddenBubbleContainerType(HiddenBubbleContainerType type) { _hideType = type; }

UInt32 LevelProperties::getClearedBoardRequiredCount() const { return _clearBoardsRequired; }
void LevelProperties::setClearedBoardRequiredCount(UInt32 amount) { _clearBoardsRequired = amount; }

BubbleColorMask LevelProperties::getEnabledColors() const { return _availableColors; }
bool LevelProperties::isColorEnabled(const BubbleColor& color) const { return _availableColors & color; }
void LevelProperties::setColorEnabled(const BubbleColor& color, bool enabled) { _availableColors = enabled ? _availableColors + color : _availableColors - color; }

RNG::Seed LevelProperties::getSeed() const { return _seed; }
bool LevelProperties::isRandomSeed() const { return _seed == 0; }
void LevelProperties::setSeed(RNG::Seed seed) { _seed = seed; _randReady = false; }
void LevelProperties::setSeedRandom() { _seed = 0; _randReady = false; }
RNG LevelProperties::generateRNG()
{
	if (!_randReady)
	{
		_rand = _seed;
		_randReady = true;
	}
	return _rand.randomRNG();
}

UInt32 LevelProperties::getInitialFilledRows() const { return _initialBubbles; }
void LevelProperties::setInitialFilledRows(UInt32 count) { _initialBubbles = count; }

bool LevelProperties::isBubbleGenerationEnabled() const { return _generateUpBubbles; }
void LevelProperties::setBubbleGenerationEnabled(bool enabled) { _generateUpBubbles = enabled; }

const RandomBubbleModelSelector& LevelProperties::getArrowModelSelector() const { return _arrowBubbleSelector; }
const RandomBubbleModelSelector& LevelProperties::getBoardModelSelector() const { return _boardBubbleSelector; }
RandomBubbleModelSelector& LevelProperties::peekArrowModelSelector() { return _arrowBubbleSelector; }
RandomBubbleModelSelector& LevelProperties::peekBoardModelSelector() { return _boardBubbleSelector; }

bool LevelProperties::isRoofEnabled() const { return _roof; }
void LevelProperties::setRoofEnabled(bool enabled) { _roof = enabled; }

bool LevelProperties::isRemoteBubblesEnabled() const { return _remote; }
void LevelProperties::setRemoteBubblesEnabled(bool enabled) { _remote = enabled; }

bool LevelProperties::isHideTimer() const { return _hideTimer; }
void LevelProperties::setHideTimer(bool enabled) { _hideTimer = enabled; }

UInt32 LevelProperties::getTimerTurnTime() const { return _timerTurnTime; }
void LevelProperties::setTimerTurnTime(UInt32 seconds) { _timerTurnTime = seconds; }

UInt32 LevelProperties::getTimerEndTime() const { return _timerEndTime; }
void LevelProperties::setTimerEndTime(UInt32 seconds) { _timerEndTime = seconds; }

TimerMode LevelProperties::getTimerMode() const { return _timerMode; }
void LevelProperties::setTimerMode(TimerMode mode) { _timerMode = mode; }

bool LevelProperties::isBubbleSwapEnabled() const { return _enableBubbleSwap; }
void LevelProperties::setBubbleSwapEnabled(bool enabled) { _enableBubbleSwap = enabled; }

const std::string& LevelProperties::getBackground() const { return _background; }
void LevelProperties::setBackground(const std::string& textureName) { _background = textureName; }

const MetaGoals& LevelProperties::getGoals() const { return _goals; }
MetaGoals& LevelProperties::peekGoals() { return _goals; }
