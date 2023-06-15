#pragma once

#include <cmath>
#include <algorithm>
#include <concepts>


namespace utils
{
	template <typename _Ty>
	concept number = std::integral<_Ty> || std::floating_point<_Ty>;
}


namespace utils
{
	using std::min;
	using std::max;

	template <number _ValTy, std::convertible_to<_ValTy> _MinTy, std::convertible_to<_ValTy> _MaxTy>
	constexpr _ValTy clamp(_ValTy value, _MinTy min, _MaxTy max)
	{
		return utils::min<_ValTy>(_ValTy(max), utils::max<_ValTy>(_ValTy(min), value));
	}

	template <std::integral _Ty>
	constexpr bool isPair(_Ty value) { return (value % _Ty(2)) == 0; }

	template <std::integral _Ty>
	constexpr bool isOdd(_Ty value) { return (value % _Ty(2)) != 0; }


	template <number _Ty>
	inline _Ty mod(_Ty left, _Ty right)
	{
		if constexpr (std::floating_point<_Ty>) return std::fmod(left, right);
		else return left % right;
	}

	//template <typename _Ty>
	//_Ty umod(_Ty left, _Ty right);



	template <number _Ty>
	inline _Ty umod(_Ty left, _Ty right)
	{
		_Ty remainder = mod(left, right);
		return remainder < 0 ? remainder + right : remainder;
	}

	template <std::floating_point _Ty>
	inline bool isAlmostEquals(_Ty left, _Ty right, _Ty epsilon = _Ty(.0000001))
	{
		return std::abs(left - right) < epsilon;
	}

	template <std::floating_point _Ty>
	inline bool isAlmostZero(_Ty value)
	{
		if constexpr (sizeof(_Ty) > 4)
			return std::abs(value) < _Ty(1e-19);
		else
			return std::abs(value) < _Ty(1e-6f);
	}

	template <std::floating_point _Ty>
	inline bool isAlmostZero(_Ty value, _Ty epsilon)
	{
		return std::abs(value) < epsilon;
	}
}
