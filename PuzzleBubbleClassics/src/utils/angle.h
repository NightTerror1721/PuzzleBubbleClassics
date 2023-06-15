#pragma once

#include "math.h"

#include <compare>
#include <concepts>
#include <numbers>

#include <SFML/Graphics.hpp>

#pragma warning(push)
#pragma warning(disable: 4244)


template <typename _Ty>
concept FloatingAngle = std::floating_point<_Ty>;

template <typename _Ty>
concept ConvertibleToFloatingAngle = FloatingAngle<_Ty> || std::integral<_Ty>;


namespace utils
{
	template <FloatingAngle _NumTy>
	inline constexpr _NumTy Pi = std::numbers::pi_v<_NumTy>;

	template <FloatingAngle _NumTy>
	inline constexpr _NumTy Pi2 = Pi<_NumTy> * _NumTy(2);

	template <FloatingAngle _NumTy>
	inline constexpr _NumTy HalfDeg = _NumTy(180);

	template <FloatingAngle _NumTy>
	inline constexpr _NumTy FullDeg = _NumTy(360);

	template <FloatingAngle _NumTy>
	inline constexpr _NumTy Deg2Rad = Pi<_NumTy> / HalfDeg<_NumTy>;

	template <FloatingAngle _NumTy>
	inline constexpr _NumTy Rad2Deg = HalfDeg<_NumTy> / Pi<_NumTy>;
}

namespace utils
{
	template <ConvertibleToFloatingAngle _NumTy>
	FloatingAngle auto convertToFloatAngle(_NumTy value)
	{
		if constexpr (FloatingAngle<_NumTy>) return value;
		else if constexpr (sizeof(_NumTy) > 4) return static_cast<double>(value);
		else return static_cast<float>(value);
	}

	template <ConvertibleToFloatingAngle _NumTy>
	using ConvertedInFloatAngleType = decltype(convertToFloatAngle(std::declval<_NumTy>()));


	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> ratioToDegrees(_NumTy ratio) { return convertToFloatAngle(ratio) * FullDeg<ConvertedInFloatAngleType<_NumTy>>; }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> ratioToRadians(_NumTy ratio) { return convertToFloatAngle(ratio) * Pi2<ConvertedInFloatAngleType<_NumTy>>; }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> degreesToRatio(_NumTy degrees) { return convertToFloatAngle(degrees) / FullDeg<ConvertedInFloatAngleType<_NumTy>>; }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> radiansToRatio(_NumTy radians) { return convertToFloatAngle(radians) * Pi2<ConvertedInFloatAngleType<_NumTy>>; }


	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> degreesToRadians(_NumTy degrees) { return convertToFloatAngle(degrees) * Deg2Rad<ConvertedInFloatAngleType<_NumTy>>; }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> radiansToDegrees(_NumTy radians) { return convertToFloatAngle(radians) * Rad2Deg<ConvertedInFloatAngleType<_NumTy>>; }
}

namespace utils
{
	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> sin01(_NumTy ratio) { return std::sin(Pi<ConvertedInFloatAngleType<_NumTy>> * convertToFloatAngle(ratio)); }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> cos01(_NumTy ratio) { return std::cos(Pi<ConvertedInFloatAngleType<_NumTy>> * convertToFloatAngle(ratio)); }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr ConvertedInFloatAngleType<_NumTy> tan01(_NumTy ratio) { return std::tan(Pi<ConvertedInFloatAngleType<_NumTy>> * convertToFloatAngle(ratio)); }
}


template <FloatingAngle _Ty>
class GenericAngle
{
public:
	using ValueType = _Ty;

public:
	static constexpr ValueType Pi = utils::Pi<ValueType>;
	static constexpr ValueType Pi2 = utils::Pi2<ValueType>;
	static constexpr ValueType Deg2Rad = utils::Deg2Rad<ValueType>;
	static constexpr ValueType Rad2Deg = utils::Rad2Deg<ValueType>;

private:
	ValueType _ratio = ValueType(0);

public:
	constexpr GenericAngle() noexcept = default;
	constexpr GenericAngle(const GenericAngle&) noexcept = default;
	constexpr GenericAngle(GenericAngle&&) noexcept = default;
	constexpr ~GenericAngle() noexcept = default;

	constexpr GenericAngle& operator= (const GenericAngle&) noexcept = default;
	constexpr GenericAngle& operator= (GenericAngle&&) noexcept = default;

	constexpr bool operator== (const GenericAngle&) const = default;
	constexpr std::partial_ordering operator<=> (const GenericAngle&) const = default;

private:
	constexpr GenericAngle(float ratio) noexcept : _ratio(ratio) {}

public:
	constexpr ValueType ratio() const { return _ratio; }
	constexpr ValueType degrees() const { return utils::ratioToDegrees(_ratio); }
	constexpr ValueType radians() const { return utils::ratioToRadians(_ratio); }

public:
	template <ConvertibleToFloatingAngle _NumTy>
	constexpr GenericAngle operator* (_NumTy right) const { return _ratio * ValueType(utils::convertToFloatAngle(right)); }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr GenericAngle operator/ (_NumTy right) const { return _ratio / ValueType(utils::convertToFloatAngle(right)); }

	inline GenericAngle operator% (GenericAngle right) const { return utils::mod(_ratio, right._ratio); }

	constexpr GenericAngle operator/ (GenericAngle right) const { return _ratio / right._ratio; }
	constexpr GenericAngle operator+ (GenericAngle right) const { return _ratio + right._ratio; }
	constexpr GenericAngle operator- (GenericAngle right) const { return _ratio - right._ratio; }

	constexpr GenericAngle operator+ () const { return *this; }
	constexpr GenericAngle operator- () const { return -_ratio; }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr GenericAngle& operator*= (_NumTy right) { return _ratio *= ValueType(utils::convertToFloatAngle(right)), *this; }

	template <ConvertibleToFloatingAngle _NumTy>
	constexpr GenericAngle& operator/= (_NumTy right) { return _ratio /= ValueType(utils::convertToFloatAngle(right)), *this; }

	inline GenericAngle& operator%= (GenericAngle right) { return _ratio = utils::mod(_ratio, right._ratio), *this; }

	constexpr GenericAngle& operator/= (GenericAngle right) { return _ratio /= right._ratio, *this; }
	constexpr GenericAngle& operator+= (GenericAngle right) { return _ratio += right._ratio, *this; }
	constexpr GenericAngle& operator-= (GenericAngle right) { return _ratio -= right._ratio, *this; }

public:
	inline ValueType sin() const { return std::sin(radians()); }
	inline ValueType cos() const { return std::cos(radians()); }
	inline ValueType tan() const { return std::tan(radians()); }

	inline ValueType asin() const { return std::asin(radians()); }
	inline ValueType acos() const { return std::acos(radians()); }
	inline ValueType atan() const { return std::atan(radians()); }

	inline ValueType sinh() const { return std::sinh(radians()); }
	inline ValueType cosh() const { return std::cosh(radians()); }
	inline ValueType tanh() const { return std::tanh(radians()); }

	inline GenericAngle abs() const { return std::abs(_ratio); }

	inline ValueType normalizedRatio() const { return utils::umod(_ratio, ValueType(1)); }

	inline GenericAngle normalized() const { return normalizedRatio(); }

	inline ValueType normalizedDegrees() const { return normalized().degrees(); }
	inline ValueType normalizedRadians() const { return normalized().radians(); }

	inline GenericAngle shortDistanceTo(GenericAngle other)
	{
		ValueType diff = utils::mod((other.normalizedRatio() - normalizedRatio() + ValueType(0.5)), ValueType(1)) - ValueType(0.5);
		return diff < ValueType(-0.5) ? GenericAngle(diff + ValueType(1)) : GenericAngle(diff);
	}

	inline GenericAngle longDistanceTo(GenericAngle other)
	{
		GenericAngle sdist = shortDistanceTo(other);
		return sdist == zero() ? zero() : sdist < zero() ? full() + sdist : -full() + sdist;
	}

	inline bool inBetween(GenericAngle min, GenericAngle max, bool inclusive)
	{
		GenericAngle nthis = normalized();
		GenericAngle nmin = min.normalized();
		GenericAngle nmax = max.normalized();
		return nmin > nmax
			? nthis >= nmin || (inclusive ? nthis <= nmax : nthis < nmax)
			: nthis >= nmin && (inclusive ? nthis <= nmax : nthis < nmax);
	}

	inline bool inBetweenInclusive(GenericAngle min, GenericAngle max) { return inBetween(min, max, true); }
	inline bool inBetweenExclusive(GenericAngle min, GenericAngle max) { return inBetween(min, max, false); }

	inline bool isAlmostEquals(GenericAngle other, ValueType epsilon = ValueType(0.0001))
	{
		return utils::isAlmostEquals(_ratio, other._ratio, epsilon);
	}

	inline bool isAlmostZero(ValueType epsilon = ValueType(0.0001)) { return utils::isAlmostZero(_ratio, epsilon); }


public:
	template <ConvertibleToFloatingAngle _NumTy>
	static constexpr GenericAngle ratio(_NumTy ratio) { return ValueType(utils::convertToFloatAngle(ratio)); }

	template <ConvertibleToFloatingAngle _NumTy>
	static constexpr GenericAngle degrees(_NumTy degrees) { return ValueType(utils::degreesToRatio(degrees)); }

	template <ConvertibleToFloatingAngle _NumTy>
	static constexpr GenericAngle radians(_NumTy radians) { return ValueType(utils::radiansToRatio(radians)); }

	template <ConvertibleToFloatingAngle _NumTy1, ConvertibleToFloatingAngle _NumTy2>
	static constexpr GenericAngle atan2(_NumTy1 x, _NumTy2 y) { return std::atan2(ValueType(utils::convertToFloatAngle(x)), ValueType(utils::convertToFloatAngle(y))); }

	template <ConvertibleToFloatingAngle _NumTy>
	static constexpr GenericAngle between(_NumTy x0, _NumTy y0, _NumTy x1, _NumTy y1)
	{
		GenericAngle angle = atan2(y1 - y0, x1 - x0);
		return angle < zero() ? angle + full() : angle;
	}

	template <ConvertibleToFloatingAngle _NumTy>
	static inline GenericAngle between(const sf::Vector2<_NumTy>& v0, const sf::Vector2<_NumTy>& v1) { return between(v0.x, v0.y, v1.x, v1.y); }

	template <ConvertibleToFloatingAngle _NumTy>
	static constexpr GenericAngle between(_NumTy ox, _NumTy oy, _NumTy x0, _NumTy y0, _NumTy x1, _NumTy y1)
	{
		return between(x0 - ox, y0 - oy, x1 - ox, y1 - oy);
	}

	template <ConvertibleToFloatingAngle _NumTy>
	static inline GenericAngle between(const sf::Vector2<_NumTy>& o, const sf::Vector2<_NumTy>& v0, const sf::Vector2<_NumTy>& v1) { return between(o.x, o.y, v0.x, v0.y, v1.x, v1.y); }

public:
	static constexpr GenericAngle epsilon() { return ValueType(0.00001); }
	static constexpr GenericAngle zero() { return ValueType(0); }
	static constexpr GenericAngle quarter() { return ValueType(0.25); }
	static constexpr GenericAngle half() { return ValueType(0.5); }
	static constexpr GenericAngle full() { return ValueType(1); }
};


namespace utils
{
	template <FloatingAngle _Ty>
	inline GenericAngle<_Ty> umod(GenericAngle<_Ty> left, GenericAngle<_Ty> right) { return GenericAngle<_Ty>::ratio(utils::umod(left.ratio(), right.ratio())); }
}


namespace std
{
	template <FloatingAngle _Ty> inline _Ty sin(GenericAngle<_Ty> angle) { return angle.sin(); }
	template <FloatingAngle _Ty> inline _Ty cos(GenericAngle<_Ty> angle) { return angle.cos(); }
	template <FloatingAngle _Ty> inline _Ty tan(GenericAngle<_Ty> angle) { return angle.tan(); }

	template <FloatingAngle _Ty> inline _Ty asin(GenericAngle<_Ty> angle) { return angle.asin(); }
	template <FloatingAngle _Ty> inline _Ty acos(GenericAngle<_Ty> angle) { return angle.acos(); }
	template <FloatingAngle _Ty> inline _Ty atan(GenericAngle<_Ty> angle) { return angle.atan(); }

	template <FloatingAngle _Ty> inline _Ty sinh(GenericAngle<_Ty> angle) { return angle.sinh(); }
	template <FloatingAngle _Ty> inline _Ty cosh(GenericAngle<_Ty> angle) { return angle.cosh(); }
	template <FloatingAngle _Ty> inline _Ty tanh(GenericAngle<_Ty> angle) { return angle.tanh(); }

	template <FloatingAngle _Ty> inline _Ty abs(GenericAngle<_Ty> angle) { return angle.abs(); }

	template <FloatingAngle _Ty>
	constexpr _Ty min(GenericAngle<_Ty> a0, GenericAngle<_Ty> a1) { return GenericAngle<_Ty>::ratio(std::min(a0.ratio(), a1.ratio())); }

	template <FloatingAngle _Ty>
	constexpr _Ty max(GenericAngle<_Ty> a0, GenericAngle<_Ty> a1) { return GenericAngle<_Ty>::ratio(std::max(a0.ratio(), a1.ratio())); }
}


using Angle = GenericAngle<float>;
using FloatAngle = GenericAngle<float>;
using DoubleAngle = GenericAngle<double>;



constexpr Angle operator"" _deg(unsigned long long value) { return Angle::degrees(value); }
constexpr Angle operator"" _deg(long double value) { return Angle::degrees(value); }

constexpr Angle operator"" _rad(unsigned long long value) { return Angle::radians(value); }
constexpr Angle operator"" _rad(long double value) { return Angle::radians(value); }

constexpr DoubleAngle operator"" _ddeg(unsigned long long value) { return DoubleAngle::degrees(value); }
constexpr DoubleAngle operator"" _ddeg(long double value) { return DoubleAngle::degrees(value); }

constexpr DoubleAngle operator"" _drad(unsigned long long value) { return DoubleAngle::radians(value); }
constexpr DoubleAngle operator"" _drad(long double value) { return DoubleAngle::radians(value); }


#pragma warning(pop)
