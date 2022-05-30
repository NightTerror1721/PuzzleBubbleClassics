#pragma once

#include <array>

#include "common.h"


class Bubble;


enum class BubbleColorType
{
	Colorless,
	NormalColor,
	MultiColor
};

enum class BounceEdge
{
	None,
	Top,
	Bottom,
	Left,
	Right
};




enum class BubbleColorCode : UInt8
{
	Colorless = 0,
	Red = 0x1 << 0,
	Orange = 0x1 << 1,
	Yellow = 0x1 << 2,
	Green = 0x1 << 3,
	Blue = 0x1 << 4,
	Purple = 0x1 << 5,
	Gray = 0x1 << 6,
	Black = 0x1 << 7,
	Multicolor = 0xff
};

static constexpr BubbleColorCode validateCode(BubbleColorCode code)
{
	using enum BubbleColorCode;
	switch (code)
	{
		case Colorless:
		case Red:
		case Orange:
		case Yellow:
		case Green:
		case Blue:
		case Purple:
		case Gray:
		case Black:
		case Multicolor:
			return code;
		default:
			return Colorless;
	}
}

template <std::integral _Ty>
static constexpr _Ty bubbleColorCodeToNumber(BubbleColorCode code)
{
	if constexpr (std::same_as<_Ty, UInt8>)
		return static_cast<_Ty>(code);
	else
		return static_cast<_Ty>(static_cast<UInt8>(code));
}

template <std::integral _Ty>
static constexpr BubbleColorCode numberToBubbleColorCode(_Ty code)
{
	if constexpr (std::same_as<_Ty, UInt8>)
		return validateCode(static_cast<BubbleColorCode>(code));
	else
		return validateCode(static_cast<BubbleColorCode>(static_cast<UInt8>(code)));
}



typedef UInt8 BubbleColorMask;

constexpr BubbleColorMask operator+ (BubbleColorMask left, BubbleColorCode right)
{
	return utils::bit::union_of(left, bubbleColorCodeToNumber<BubbleColorMask>(validateCode(right)));
}
constexpr BubbleColorMask operator+ (BubbleColorCode left, BubbleColorMask right)
{
	return utils::bit::union_of(right, bubbleColorCodeToNumber<BubbleColorMask>(validateCode(left)));
}

constexpr BubbleColorMask operator- (BubbleColorMask left, BubbleColorCode right)
{
	return utils::bit::disjuntion_of(left, bubbleColorCodeToNumber<BubbleColorMask>(right));
}
constexpr BubbleColorMask operator- (BubbleColorCode left, BubbleColorMask right)
{
	return utils::bit::disjuntion_of(right, bubbleColorCodeToNumber<BubbleColorMask>(left));
}

constexpr bool operator& (BubbleColorMask left, BubbleColorCode right)
{
	return utils::bit::check_all(left, bubbleColorCodeToNumber<BubbleColorMask>(right));
}
constexpr bool operator& (BubbleColorCode left, BubbleColorMask right)
{
	return utils::bit::check_all(right, bubbleColorCodeToNumber<BubbleColorMask>(left));
}

constexpr BubbleColorMask& operator+= (BubbleColorMask& left, BubbleColorCode right)
{
	return left = left + right;
}
constexpr BubbleColorMask& operator-= (BubbleColorMask& left, BubbleColorCode right)
{
	return left = left - right;
}




class BubbleColor
{
private:
	BubbleColorCode _code = BubbleColorCode::Colorless;

public:
	constexpr BubbleColor() = default;
	constexpr BubbleColor(const BubbleColor&) = default;
	constexpr BubbleColor(BubbleColor&&) noexcept = default;
	constexpr ~BubbleColor() = default;

	constexpr BubbleColor& operator= (const BubbleColor&) = default;
	constexpr BubbleColor& operator= (BubbleColor&&) noexcept = default;

	constexpr bool operator== (const BubbleColor&) const = default;
	constexpr auto operator<=> (const BubbleColor&) const = default;

public:
	constexpr BubbleColor(BubbleColorCode code) : _code(validateCode(code)) {}

	constexpr bool isColorless() const { return _code == BubbleColorCode::Colorless; }
	constexpr bool isRed() const { return _code == BubbleColorCode::Red; }
	constexpr bool isOrange() const { return _code == BubbleColorCode::Orange; }
	constexpr bool isYellow() const { return _code == BubbleColorCode::Yellow; }
	constexpr bool isGreen() const { return _code == BubbleColorCode::Green; }
	constexpr bool isBlue() const { return _code == BubbleColorCode::Blue; }
	constexpr bool isPurple() const { return _code == BubbleColorCode::Purple; }
	constexpr bool isGray() const { return _code == BubbleColorCode::Gray; }
	constexpr bool isBlack() const { return _code == BubbleColorCode::Black; }
	constexpr bool isMulticolor() const { return _code == BubbleColorCode::Multicolor; }

	friend constexpr bool operator== (BubbleColor color, BubbleColorCode code) { return color._code == code; }
	friend constexpr bool operator!= (BubbleColor color, BubbleColorCode code) { return color._code != code; }
	friend constexpr bool operator== (BubbleColorCode code, BubbleColor color) { return code == color._code; }
	friend constexpr bool operator!= (BubbleColorCode code, BubbleColor color) { return code != color._code; }

	constexpr BubbleColorCode code() const { return _code; }

public:
	constexpr String name() const
	{
		using enum BubbleColorCode;
		switch (_code)
		{
			default:
			case Colorless: return "Colorless";
			case Red: return "Red";
			case Orange: return "Orange";
			case Yellow: return "Yellow";
			case Green: return "Green";
			case Blue: return "Blue";
			case Purple: return "Purple";
			case Gray: return "Gray";
			case Black: return "Black";
			case Multicolor: return "Multicolor";
		}
	}

public:
	static constexpr Size count = 10;
	static constexpr Size single_color_count = 8;

	static const BubbleColor Colorless;
	static const BubbleColor Red;
	static const BubbleColor Orange;
	static const BubbleColor Yellow;
	static const BubbleColor Green;
	static const BubbleColor Blue;
	static const BubbleColor Purple;
	static const BubbleColor Gray;
	static const BubbleColor Black;
	static const BubbleColor Multicolor;

	static constexpr std::array<BubbleColor, count> all()
	{
		return { Colorless, Red, Orange, Yellow, Green, Blue, Purple, Gray, Black, Multicolor };
	}

	static constexpr std::array<BubbleColor, count> singleColor()
	{
		return { Red, Orange, Yellow, Green, Blue, Purple, Gray, Black };
	}

	static constexpr BubbleColor defaultColor() { return Blue; }

public:
	constexpr friend BubbleColorMask operator+ (BubbleColorMask left, BubbleColor right) { return left + right._code; }
	constexpr friend BubbleColorMask operator+ (BubbleColor left, BubbleColorMask right) { return left._code + right; }

	constexpr friend BubbleColorMask operator- (BubbleColorMask left, BubbleColor right) { return left - right._code; }
	constexpr friend BubbleColorMask operator- (BubbleColor left, BubbleColorMask right) { return left._code - right; }

	constexpr friend bool operator& (BubbleColorMask left, BubbleColor right) { return left & right._code; }
	constexpr friend bool operator& (BubbleColor left, BubbleColorMask right) { return left._code & right; }

	constexpr friend BubbleColorMask& operator+= (BubbleColorMask& left, BubbleColor right) { return left += right._code; }
	constexpr friend BubbleColorMask& operator-= (BubbleColorMask& left, BubbleColor right) { return left -= right._code; }
};







struct BubbleModel
{
	/* PROPERTIES */
	String name;

	BubbleColor color;

	bool floating;
	bool destroyInBottom;
	bool requireDestroyToClear;
	bool onlyBoardColorInArrowGen;

	UInt8 resistence;

	float pointsOfTurnsToDown;


	/* FUNCTIONS */
	std::function<void(std::shared_ptr<Bubble>, BubbleColor, bool)> init;

	std::function<void(Bubble*, Bubble*)> onCollide;
	std::function<void(Bubble*)> onInserted;
	std::function<void(Bubble*)> onExplode;
	std::function<void(Bubble*, Bubble*)> onNeighborInserted;
	std::function<void(Bubble*, Bubble*)> onNeighborExplode;

	bool operator== (const BubbleModel& right);
	bool operator!= (const BubbleModel& right);
};


