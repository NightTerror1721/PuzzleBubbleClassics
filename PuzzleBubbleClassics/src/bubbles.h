#pragma once

#include <array>

#include "common.h"
#include "game_basics.h"
#include "assets.h"
#include "lua.h"
#include "utils/SimpleElementManager.h"
#include "utils/RNG.h"


class Bubble;


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

	template <std::integral _Ty = int>
	constexpr _Ty asNumber() const { return bubbleColorCodeToNumber<_Ty>(_code); }

	bool matches(BubbleColor other) const;

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

	bool colorless = true;
	bool multicolor = false;

	bool floating = false;
	bool destroyInBottom = false;
	bool requireDestroyToClear = false;
	bool onlyBoardColorInArrowGen = false;

	int resistence = 0;

	float pointsOfTurnsToDown = 0;


	/* FUNCTIONS */
	std::unique_ptr<LuaRef> onInit; // void init(Bubble bubble, BubbleColor color, bool editorMode); //

	std::unique_ptr<LuaRef> onCollide; // void onCollide(Bubble self, Bubble other); //
	std::unique_ptr<LuaRef> onInserted; // void onInserted(Bubble self); //
	std::unique_ptr<LuaRef> onExplode; // void onExplode(Bubble self); //
	std::unique_ptr<LuaRef> onNeighborInserted; // void onNeighborInserted(Bubble self, Bubble other); //
	std::unique_ptr<LuaRef> onNeighborExplode; // void onNeighborExplode(Bubble self, Bubble other); //

	inline bool operator== (const BubbleModel& right) { return name == right.name; }
	inline bool operator!= (const BubbleModel& right) { return name != right.name; }

public:
	BubbleModel() = default;
	BubbleModel(const BubbleModel&) = delete;
	BubbleModel(BubbleModel&&) noexcept = default;
	~BubbleModel() = default;

	BubbleModel& operator= (const BubbleModel&) = delete;
	BubbleModel& operator= (BubbleModel&&) noexcept = default;

	void callOnInit(const std::shared_ptr<Bubble>& bubble, BubbleColor color, bool editorMode);
	void callOnCollide(const std::shared_ptr<Bubble>& self, const std::shared_ptr<Bubble>& other);
	void callOnInserted(const std::shared_ptr<Bubble>& self);
	void callOnExplode(const std::shared_ptr<Bubble>& self);
	void callOnNeighborInserted(const std::shared_ptr<Bubble>& self, const std::shared_ptr<Bubble>& other);
	void callOnNeighborExplode(const std::shared_ptr<Bubble>& self, const std::shared_ptr<Bubble>& other);


private:
	void lua_SetEvent(std::unique_ptr<LuaRef>& event, const LuaRef& func, LuaState* state);
	LuaRef lua_GetEvent(const std::unique_ptr<LuaRef>& event, LuaState* state) const;

public:
	inline void lua_SetOnInit(LuaRef func, LuaState* state) { lua_SetEvent(onInit, func, state); }
	inline void lua_SetOnCollide(LuaRef func, LuaState* state) { lua_SetEvent(onCollide, func, state); }
	inline void lua_SetOnInserted(LuaRef func, LuaState* state) { lua_SetEvent(onInserted, func, state); }
	inline void lua_SetOnExplode(LuaRef func, LuaState* state) { lua_SetEvent(onExplode, func, state); }
	inline void lua_SetOnNeighborInserted(LuaRef func, LuaState* state) { lua_SetEvent(onNeighborInserted, func, state); }
	inline void lua_SetOnNeighborExplode(LuaRef func, LuaState* state) { lua_SetEvent(onNeighborExplode, func, state); }

	inline LuaRef lua_GetOnInit(LuaState* state) const { return lua_GetEvent(onInit, state); }
	inline LuaRef lua_GetOnCollide(LuaState* state) const { return lua_GetEvent(onCollide, state); }
	inline LuaRef lua_GetOnInserted(LuaState* state) const { return lua_GetEvent(onInserted, state); }
	inline LuaRef lua_GetOnExplode(LuaState* state) const { return lua_GetEvent(onExplode, state); }
	inline LuaRef lua_GetOnNeighborInserted(LuaState* state) const { return lua_GetEvent(onNeighborInserted, state); }
	inline LuaRef lua_GetOnNeighborExplode(LuaState* state) const { return lua_GetEvent(onNeighborExplode, state); }
};






class BouncingBounds
{
private:
	Bubble& _bubble;
	sf::IntRect _bounds;
	bool _top = false;
	bool _bottom = false;

public:
	BouncingBounds() = delete;
	BouncingBounds(const BouncingBounds&) = default;
	BouncingBounds(BouncingBounds&&) noexcept = default;
	~BouncingBounds() = default;

	BouncingBounds& operator= (const BouncingBounds&) = default;
	BouncingBounds& operator= (BouncingBounds&&) noexcept = default;

private:
	inline BouncingBounds(Bubble& bubble) : _bubble(bubble), _bounds() {}

public:
	inline void setBounds(const sf::IntRect& bounds) { _bounds = bounds; }
	void setTopEnabled(bool enabled) { _top = enabled; }
	void setBottomEnabled(bool enabled) { _bottom = enabled; }

	BounceEdge check();

	friend Bubble;
};






class Bubble : public GameObject, public sf::Transformable
{
public:
	static constexpr UInt32 radius = 32;
	static constexpr UInt32 hitboxWith = 64;
	static constexpr UInt32 hitboxHeight = 54;

private:
	std::shared_ptr<BubbleModel> _model;

	bool _exploited;

	Vector2f _speed;
	Vector2f _acceleration;

	Vector2f _allocPosition;
	Vector2i _allocCell;
	bool _floatingCheckPhase;

	BubbleColor _color;

	AnimatedSprite _sprite;

	BouncingBounds _bounce;

public:
	Bubble(const std::shared_ptr<BubbleModel>& model, TextureManager& textures = TextureManager::root());
	virtual ~Bubble();

	const std::shared_ptr<BubbleModel>& getModel() const;

	bool hasExploited() const;
	void explode();

	void setSpeed(const Vector2f& speed);
	const Vector2f& getSpeed() const;

	void setAcceleration(const Vector2f& acceleration);
	const Vector2f& getAcceleration() const;

	void translate(const Vector2f& dp);
	void translate(float dx, float dy);
	void move(const Vector2f& speed, const Vector2f& acceleration = {});

	BubbleColor getColor() const;

	bool colorMatch(const std::shared_ptr<Bubble>& other) const;

	AnimatedSprite* getSprite();
	const AnimatedSprite* getSprite() const;


	/* Model functions */
	Int8 getResistence() const;
	bool isIndestructible() const;
	bool isFloating() const;
	bool destroyInBottom() const;
	bool requireDestroyToClear() const;
	float getPointsOfTurnsToDown() const;
};







class BubbleIdentifier
{
private:
	String _model;
	BubbleColor _color;

public:
	BubbleIdentifier() = default;
	BubbleIdentifier(const BubbleIdentifier&) = default;
	BubbleIdentifier(BubbleIdentifier&&) noexcept = default;
	~BubbleIdentifier() = default;

	BubbleIdentifier& operator= (const BubbleIdentifier&) = default;
	BubbleIdentifier& operator= (BubbleIdentifier&&) noexcept = default;

	bool operator== (const BubbleIdentifier&) const = default;

public:
	constexpr BubbleIdentifier(const String& modelName, BubbleColor color) : _model(modelName), _color(color) {}

	constexpr bool isValid() const { return !_model.empty(); }

	constexpr const String& getModelName() const { return _model; }
	constexpr BubbleColor getColor() const { return _color; }

	constexpr void setModelName(const String& modelName) { _model = modelName; }
	constexpr void setColor(BubbleColor color) { _color = color; }

public:
	constexpr operator bool() const { return !_model.empty(); }
	constexpr bool operator! () const { return _model.empty(); }

};







class BubbleModelManager
{
private:
	std::unordered_map<String, std::shared_ptr<BubbleModel>> _models;
	std::shared_ptr<BubbleModel> _defaultModel;



public:
	BubbleModelManager() = default;
	BubbleModelManager(const BubbleModelManager&) = delete;
	BubbleModelManager(BubbleModelManager&&) noexcept = default;
	~BubbleModelManager() = default;

	BubbleModelManager& operator= (const BubbleModelManager&) = delete;
	BubbleModelManager& operator= (BubbleModelManager&&) noexcept = default;

public:
	std::shared_ptr<BubbleModel> createModel(const String& name);
	std::shared_ptr<BubbleModel> getModel(const String& name) const;
	bool hasModel(const String& name) const;
	std::shared_ptr<BubbleModel> getDefaultModel() const;
	void setDefaultModel(const String& name);

	std::shared_ptr<Bubble> createBubble(TextureManager& textures, bool editorMode, const String& modelName, BubbleColor color = BubbleColor::defaultColor()) const;

public:
	inline std::shared_ptr<Bubble> createBubble(TextureManager& textures, bool editorMode, const BubbleIdentifier& identifier)
	{
		return createBubble(textures, editorMode, identifier.getModelName(), identifier.getColor());
	}

public:
	static BubbleModelManager instance;
};






class RandomBubbleModelSelector
{
public:
	static constexpr UInt16 maxScore = 1000;
	static constexpr UInt16 minScore = 0;

private:
	std::unordered_map<String, UInt16> _models;
	mutable RNG::RandomValue _score;
	mutable bool _recompute = true;

public:
	RandomBubbleModelSelector() = default;
	RandomBubbleModelSelector(const RandomBubbleModelSelector&) = default;
	RandomBubbleModelSelector(RandomBubbleModelSelector&&) = default;
	~RandomBubbleModelSelector() = default;

	RandomBubbleModelSelector& operator= (const RandomBubbleModelSelector&) = default;
	RandomBubbleModelSelector& operator= (RandomBubbleModelSelector&&) = default;

public:
	void setModelScore(const String& modelName, UInt16 score);
	UInt16 getModelScore(const String& modelName) const;

	std::shared_ptr<BubbleModel> selectModel(RNG& rand) const;

private:
	void computeScore() const;
};
