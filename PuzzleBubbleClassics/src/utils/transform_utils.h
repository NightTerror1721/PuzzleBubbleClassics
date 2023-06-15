#pragma once

#include "rawtypes.h"
#include <SFML/Graphics.hpp>


template <typename _Ty> requires std::integral<_Ty> || std::floating_point<_Ty>
struct Dimensions2
{
	using ValueType = _Ty;

	ValueType width;
	ValueType height;
};

template <typename _Ty> requires std::integral<_Ty> || std::floating_point<_Ty>
struct Point2
{
	using ValueType = _Ty;

	ValueType x;
	ValueType y;
};

template <typename _Ty> requires std::integral<_Ty> || std::floating_point<_Ty>
struct Bounds2
{
	using ValueType = _Ty;

	ValueType x;
	ValueType y;
	ValueType width;
	ValueType height;
};



class SizedTransformable : public sf::Transformable
{
public:
	using Dimension = Dimensions2<float>;

private:
	Dimension _size;

public:
	SizedTransformable() = default;
	SizedTransformable(const SizedTransformable&) = default;
	SizedTransformable(SizedTransformable&&) noexcept = default;
	virtual ~SizedTransformable() = default;

	SizedTransformable& operator= (const SizedTransformable&) = default;
	SizedTransformable& operator= (SizedTransformable&&) noexcept = default;

public:
	constexpr const Dimension& getSize() const { return _size; }
	inline sf::Vector2f getSizeAsVector() const { return { _size.width, _size.height }; }
	constexpr void setSize(float width, float height) { _size.width = width < 0 ? -width : width, _size.height = height < 0 ? -height : height; }
	constexpr void setSize(const Dimension& size) { setSize(size.width, size.height); }
	constexpr void setSize(const Bounds2<float>& bounds) { setSize(bounds.width, bounds.height); }
	inline void setSize(const sf::Vector2f& size) { setSize(size.x, size.y); }

	constexpr float getWidth() const { return _size.width; }
	constexpr float getHeight() const { return _size.height; }
};
