#pragma once

#include "object_basics.h"
#include "sprite.h"
#include "motion.h"
#include "lua/template.h"

#include "utils/rng.h"

#include <concepts>
#include <compare>
#include <array>


class Bubble;
class BubbleBoardCell;
class MetaBubble;


enum class EdgeBounce
{
	None,
	Top,
	Right,
	Bottom,
	Left
};


enum class BubbleColorCode : Uint8
{
	Red		= 1,
	Orange	= 2,
	Yellow	= 3,
	Green	= 4,
	Blue	= 5,
	Purple	= 6,
	Gray	= 7,
	Black	= 8,

	Colorless = 0,
	Multicolor = 255,

	FirstColor = Red,
	LastColor = Black,

	Default = Blue
};

namespace utils::bubble
{
	static constexpr BubbleColorCode validateColorCode(BubbleColorCode code)
	{
		return code > BubbleColorCode::LastColor ? BubbleColorCode::Multicolor : code;
	}

	template <std::integral _Ty>
	static constexpr _Ty bubbleColorCodeToNumber(BubbleColorCode code) { return _Ty(Uint8(code)); }

	template <>
	static constexpr Uint8 bubbleColorCodeToNumber<Uint8>(BubbleColorCode code) { return Uint8(code); }

	template <std::integral _Ty>
	static constexpr BubbleColorCode numberToBubbleColorCode(_Ty n) { return BubbleColorCode(Uint8(n)); }

	template <>
	static constexpr BubbleColorCode numberToBubbleColorCode<Uint8>(Uint8 n) { return BubbleColorCode(n); }

	static constexpr bool bubbleColorCodeMatches(BubbleColorCode c1, BubbleColorCode c2)
	{
		if (c1 == BubbleColorCode::Colorless || c2 == BubbleColorCode::Colorless)
			return false;

		if (c1 == c2)
			return true;

		return c1 == BubbleColorCode::Multicolor || c2 == BubbleColorCode::Multicolor;
	}
}


class BubbleColor
{
private:
	BubbleColorCode _code = BubbleColorCode::Colorless;

public:
	constexpr BubbleColor() noexcept = default;
	constexpr BubbleColor(const BubbleColor&) noexcept = default;
	constexpr BubbleColor(BubbleColor&&) noexcept = default;
	constexpr ~BubbleColor() noexcept = default;

	constexpr BubbleColor& operator= (const BubbleColor&) noexcept = default;
	constexpr BubbleColor& operator= (BubbleColor&&) noexcept = default;

	constexpr bool operator== (const BubbleColor&) const noexcept = default;
	constexpr std::strong_ordering operator<=> (const BubbleColor&) const noexcept = default;

public:
	constexpr explicit BubbleColor(BubbleColorCode code) noexcept : _code(utils::bubble::validateColorCode(code)) {}

	constexpr bool isColorless() const noexcept { return _code == BubbleColorCode::Colorless; }
	constexpr bool isRed() const noexcept { return _code == BubbleColorCode::Red; }
	constexpr bool isOrange() const noexcept { return _code == BubbleColorCode::Orange; }
	constexpr bool isYellow() const noexcept { return _code == BubbleColorCode::Yellow; }
	constexpr bool isGreen() const noexcept { return _code == BubbleColorCode::Green; }
	constexpr bool isBlue() const noexcept { return _code == BubbleColorCode::Blue; }
	constexpr bool isPurple() const noexcept { return _code == BubbleColorCode::Purple; }
	constexpr bool isGray() const noexcept { return _code == BubbleColorCode::Gray; }
	constexpr bool isBlack() const noexcept { return _code == BubbleColorCode::Black; }
	constexpr bool isMulticolor() const noexcept { return _code == BubbleColorCode::Multicolor; }
	constexpr bool isNormalColor() const noexcept { return _code >= BubbleColorCode::FirstColor && _code <= BubbleColorCode::LastColor; }

	friend constexpr bool operator== (BubbleColor left, BubbleColorCode right) noexcept { return left._code == right; }
	friend constexpr bool operator== (BubbleColorCode left, BubbleColor right) noexcept { return left == right._code; }

	friend constexpr std::strong_ordering operator<=> (BubbleColor left, BubbleColorCode right) noexcept { return left._code <=> right; }
	friend constexpr std::strong_ordering operator<=> (BubbleColorCode left, BubbleColor right) noexcept { return left <=> right._code; }

	constexpr BubbleColorCode code() const noexcept { return _code; }

	template <std::integral _Ty>
	constexpr _Ty number() const noexcept { return utils::bubble::bubbleColorCodeToNumber(_code); }

	constexpr bool matches(BubbleColor other) const noexcept { return utils::bubble::bubbleColorCodeMatches(_code, other._code); }

	constexpr bool matches(BubbleColorCode otherCode) const noexcept { return utils::bubble::bubbleColorCodeMatches(_code, otherCode); }

	static constexpr bool matches(BubbleColor c1, BubbleColor c2) noexcept { return utils::bubble::bubbleColorCodeMatches(c1._code, c2._code); }
	static constexpr bool matches(BubbleColor c1, BubbleColorCode c2) noexcept { return utils::bubble::bubbleColorCodeMatches(c1._code, c2); }
	static constexpr bool matches(BubbleColorCode c1, BubbleColor c2) noexcept { return utils::bubble::bubbleColorCodeMatches(c1, c2._code); }
	static constexpr bool matches(BubbleColorCode c1, BubbleColorCode c2) noexcept { return utils::bubble::bubbleColorCodeMatches(c1, c2); }

	constexpr std::string_view name() const noexcept
	{
		using enum BubbleColorCode;
		switch (_code)
		{
			case Colorless: return "Colorless";
			case Red: return "Red";
			case Orange: return "Orange";
			case Yellow: return "Yellow";
			case Green: return "Green";
			case Blue: return "Blue";
			case Purple: return "Purple";
			case Gray: return "Gray";
			case Black: return "Black";
			case Multicolor:
			default: return "Multicolor";
		}
	}

public:
	static constexpr std::size_t Count = std::size_t(BubbleColorCode::LastColor) - std::size_t(BubbleColorCode::FirstColor) + 1;

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
	static const BubbleColor Default;

	static constexpr std::array<BubbleColor, Count> all()
	{
		return { Red, Orange, Yellow, Green, Blue, Purple, Gray, Black };
	}
};

inline constexpr const BubbleColor BubbleColor::Colorless{ BubbleColorCode::Colorless };
inline constexpr const BubbleColor BubbleColor::Red{ BubbleColorCode::Red };
inline constexpr const BubbleColor BubbleColor::Orange{ BubbleColorCode::Orange };
inline constexpr const BubbleColor BubbleColor::Yellow{ BubbleColorCode::Yellow };
inline constexpr const BubbleColor BubbleColor::Green{ BubbleColorCode::Green };
inline constexpr const BubbleColor BubbleColor::Blue{ BubbleColorCode::Blue };
inline constexpr const BubbleColor BubbleColor::Purple{ BubbleColorCode::Purple };
inline constexpr const BubbleColor BubbleColor::Gray{ BubbleColorCode::Gray };
inline constexpr const BubbleColor BubbleColor::Black{ BubbleColorCode::Black };
inline constexpr const BubbleColor BubbleColor::Multicolor{ BubbleColorCode::Multicolor };
inline constexpr const BubbleColor BubbleColor::Default{ BubbleColorCode::Default };



class BouncingBounds
{
public:
	friend Bubble;

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
	BouncingBounds(Bubble& bubble) : _bubble(bubble), _bounds() {}

public:
	constexpr void setBounds(const sf::IntRect& bounds) { _bounds = bounds; }

	constexpr void setTopEnabled(bool enabled) { _top = enabled; }
	constexpr void setBottomEnabled(bool enabled) { _bottom = enabled; }

public:
	EdgeBounce check();
};



class BubbleModel : public LuaTemplate
{
private:
	static constexpr std::string_view FunctionOnConstruct = "OnConstruct";
	static constexpr std::string_view FunctionOnCollide = "OnCollide";
	static constexpr std::string_view FunctionOnInserted = "OnInserted";
	static constexpr std::string_view FunctionOnExplode = "OnExplode";
	static constexpr std::string_view FunctionOnNeighborInserted = "OnNeighborInserted";
	static constexpr std::string_view FunctionOnNeighborExplode = "OnNeighborExplode";
	static constexpr std::string_view FunctionOnIsDefaultModel = "OnIsDefaultModel";

public:
	BubbleModel() = default;
	BubbleModel(const BubbleModel&) = delete;
	BubbleModel(BubbleModel&&) noexcept = default;
	~BubbleModel() = default;

	BubbleModel& operator= (const BubbleModel&) = delete;
	BubbleModel& operator= (BubbleModel&&) noexcept = default;

public:
	inline void onConstruct(const std::shared_ptr<Bubble>& bubble, BubbleColor color, bool editorMode)
	{
		vcall(FunctionOnConstruct, bubble.get(), color, editorMode);
	}

	inline void onCollide(const std::shared_ptr<Bubble>& projectile, const std::shared_ptr<Bubble>& target)
	{
		vcall(FunctionOnCollide, projectile.get(), target.get());
	}

	inline void onInserted(const std::shared_ptr<Bubble>& bubble)
	{
		vcall(FunctionOnInserted, bubble.get());
	}

	inline void onExplode(const std::shared_ptr<Bubble>& bubble)
	{
		vcall(FunctionOnExplode, bubble.get());
	}

	inline void onNeighborInserted(const std::shared_ptr<Bubble>& bubble, const std::shared_ptr<Bubble>& neighbor)
	{
		vcall(FunctionOnNeighborInserted, bubble.get(), neighbor.get());
	}

	inline void onNeighborExplode(const std::shared_ptr<Bubble>& bubble, const std::shared_ptr<Bubble>& neighbor)
	{
		vcall(FunctionOnNeighborExplode, bubble.get(), neighbor.get());
	}

	inline bool onIsDefaultModel()
	{
		return call<bool>(FunctionOnIsDefaultModel);
	}
};


class BubbleModelManager : public LuaTemplateManager<BubbleModel>
{
private:
	static BubbleModelManager Instance;

private:
	std::shared_ptr<BubbleModel> _defaultModel = nullptr;

public:
	std::shared_ptr<BubbleModel> load(const std::string_view name);

	void loadAllModels();

public:
	constexpr const std::shared_ptr<BubbleModel>& getDefaultModel() { return _defaultModel; }

public:
	static constexpr BubbleModelManager& instance() { return Instance; }
};



class Bubble : public GameObject, public SpriteMotionObject
{
public:
	static constexpr float Diameter = 45;
	static constexpr float Radius = Diameter / 2.f;
	static constexpr float HitboxWidth = Diameter;
	static constexpr float HitboxHeight = 39;

	friend BubbleBoardCell;

private:
	std::shared_ptr<BubbleModel> _model;

	bool _exploited = false;

	sf::Vector2f _allocPosition;
	sf::Vector2i _allocCell;
	bool _floatingCheckPhase = false;

	bool _colorless = true;
	bool _multicolor = false;

	bool _floating = false;
	bool _destroyInBottom = false;
	bool _requireDestroyToClear = false;
	bool _onlyBoardColorInArrowGen = false;

	int _resistance = 0;

	float _pointsOfTurnsToDown = 0;

	BubbleColor _color;

	BouncingBounds _bounce;

	Reference<BubbleBoardCell> _cell = nullptr;

public:
	Bubble(const Bubble&) = delete;
	Bubble(Bubble&&) noexcept = default;

	Bubble& operator= (const Bubble&) = delete;
	Bubble& operator= (Bubble&&) = default;

	~Bubble() = default;

private:
	Bubble();

public:
	static std::shared_ptr<Bubble> make(const std::shared_ptr<BubbleModel>& model, BubbleColor color = BubbleColor::Colorless, bool editorMode = false);
	static std::shared_ptr<Bubble> make(std::string_view model, BubbleColor color = BubbleColor::Colorless, bool editorMode = false);

public:
	constexpr const std::shared_ptr<BubbleModel>& getModel() const { return _model; }

	constexpr bool hasExploited() const { return _exploited; }

	inline void translate(const sf::Vector2f& delta) { setPosition(getPosition() + delta); }
	inline void translate(float dx, float dy) { translate({ dx, dy }); }
	inline void move(const sf::Vector2f& speed, const sf::Vector2f& acceleration = {}) { setSpeed(speed), setAcceleration(acceleration); }

	constexpr void setColor(BubbleColor color) { _color; }
	constexpr BubbleColor getColor() const { return _color; }

	constexpr bool colorMatches(const std::shared_ptr<Bubble>& other) const { return _color.matches(other->_color); }

	constexpr int getResistance() const { return _resistance; }
	constexpr bool isIndestructible() const { return _resistance < 0; }
	constexpr bool isFloating() const { return _floating; }
	constexpr bool destroyInBottom() const { return _destroyInBottom; }
	constexpr bool requireDestroyToClear() const { return _requireDestroyToClear; }
	constexpr bool getPointsOfTurnsToDown() const { return _pointsOfTurnsToDown; }

	constexpr float getDiameter() const { return Diameter; }
	constexpr float getRadius() const { return Radius; }
	constexpr float getHitboxWidth() const { return HitboxWidth; }
	constexpr float getHitboxHeight() const { return HitboxHeight; }

	inline sf::Vector2f getBoundsSize() const { return { Diameter, Diameter }; }
	inline sf::Vector2f getHitboxSize() const { return { HitboxWidth, HitboxHeight }; }

	constexpr Reference<BubbleBoardCell> getCell() const { return _cell; }
	constexpr bool hasCell() const { return _cell != nullptr; }

private:
	constexpr void setCell(Reference<BubbleBoardCell> cell) { _cell = cell; }

public:
	constexpr MetaBubble getMetaBubble() const;

public:
	void render(sf::RenderTarget& target, sf::RenderStates rs) override;
	void update(const sf::Time& elapsedTime) override;
};



class MetaBubble
{
private:
	std::string _model;
	BubbleColor _color;
	bool _randomColor = false;

public:
	constexpr MetaBubble() = default;
	constexpr MetaBubble(const MetaBubble&) = default;
	constexpr MetaBubble(MetaBubble&&) noexcept = default;
	constexpr ~MetaBubble() = default;

	constexpr MetaBubble& operator= (const MetaBubble&) = default;
	constexpr MetaBubble& operator= (MetaBubble&&) noexcept = default;

	constexpr bool operator== (const MetaBubble&) const = default;
	constexpr std::strong_ordering operator<=> (const MetaBubble&) const = default;

public:
	constexpr MetaBubble(const std::string& bubbleModelName, BubbleColor color) : _model(bubbleModelName), _color(color), _randomColor(false) {}
	constexpr MetaBubble(const std::string& bubbleModelName) : _model(bubbleModelName), _color(), _randomColor(true) {}

	constexpr bool isValid() const { return !_model.empty(); }

	constexpr const std::string& getModelName() const { return _model; }
	constexpr BubbleColor getColor() const { return _color; }
	constexpr bool hasRandomColor() const { return _randomColor; }

	inline std::shared_ptr<BubbleModel> getModel() const { return BubbleModelManager::instance().get(_model); }

	constexpr void setModelName(std::string_view bubbleModelName) { _model = bubbleModelName; }
	constexpr void setColor(BubbleColor color) { _color = color, _randomColor = false; }
	constexpr void setRandomColor() { _color = BubbleColor::Colorless, _randomColor = true; }

public:
	constexpr operator bool() const { return !_model.empty(); }
	constexpr bool operator! () const { return _model.empty(); }

public:
	friend std::hash<MetaBubble>;
};

namespace std
{
	template <>
	struct std::hash<MetaBubble> : public std::hash<std::string>
	{
		inline std::size_t operator() (const MetaBubble& id) const noexcept
		{
			return std::hash<std::string>::_Do_hash(id._model) + std::size_t(id._color.code());
		}
	};
}


constexpr MetaBubble Bubble::getMetaBubble() const { return { _model->getName(), _color }; }
