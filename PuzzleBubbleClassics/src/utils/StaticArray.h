#pragma once

#include <concepts>
#include <algorithm>
#include <new>

#include "ArrayIterator.h"
#include "ArrayView.h"


template <typename _Ty, std::unsigned_integral _SizeTy = std::size_t> requires
std::default_initializable<_Ty>&& std::destructible<_Ty>
class StaticArray
{
public:
	using SubscribedType = _Ty;
	using SubscribedReferenceType = _Ty&;
	using ConstSubscribedReferenceType = const _Ty&;
	using SubscribedRValueType = _Ty&&;
	using SubscribedPointerType = _Ty*;
	using ConstSubscribedPointerType = const _Ty*;
	using ArrayType = _Ty*;
	using ConstArrayType = const _Ty*;
	using SizeType = _SizeTy;
	using IndexType = _SizeTy;
	using iterator = ArrayIterator<SubscribedType>;
	using const_iterator = ConstArrayIterator<SubscribedType>;

private:
	ArrayType _array = nullptr;
	SizeType _size = 0;

public:
	constexpr StaticArray() = default;

public:
	constexpr StaticArray(SizeType size) : _array(size > 0 ? _newArray(size) : nullptr), _size(size) {}
	constexpr StaticArray(const StaticArray& other) requires std::copyable<SubscribedType> :
		StaticArray(other._size) { _copydata(_array, other._array); }
	constexpr StaticArray(StaticArray&& other) noexcept :
		StaticArray() {
		_move(_array, other._array);
	}
	constexpr ~StaticArray() { _destroy(); }

	constexpr StaticArray& operator= (const StaticArray& right)
	{
		_destroy();
		new (this) StaticArray(right._size);
		if (_array)
			_copydata(_array, right._size);
		return *this;
	}

	constexpr StaticArray& operator= (StaticArray&& right) noexcept { return _destroy(), _move(*this, right), * this; }

	constexpr bool operator== (const StaticArray& right) requires std::equality_comparable<SubscribedType>
	{
		if (this == std::addressof(right))
			return true;

		if (_size != right._size)
			return false;

		SizeType len = _size;
		for (SizeType i = 0; i < len; ++i)
			if (!(_array[i] == right._array[i]))
				return false;
		return true;
	}
	constexpr bool operator!= (const StaticArray& right) requires std::equality_comparable<SubscribedType>
	{
		if (this == std::addressof(right))
			return false;

		if (_size != right._size)
			return true;

		SizeType len = _size;
		for (SizeType i = 0; i < len; ++i)
			if (!(_array[i] == right._array[i]))
				return true;
		return false;
	}

	constexpr operator ArrayView<SubscribedType, SizeType>() const { return { _array, _size }; }
	constexpr operator const ArrayView<const SubscribedType, SizeType>() const { return { _array, _size }; }

public:
	constexpr SizeType size() const { return _size; }
	constexpr bool empty() const { return !_array; }

	constexpr ArrayView<SubscribedType, SizeType> view() const { return { _array, _size }; }

	constexpr ArrayView<SubscribedType, SizeType> view(IndexType from)
	{
		if (from == _size)
			return {};

		_checkRange(from);
		return { _array + from, _size - from };
	}

	constexpr ArrayView<SubscribedType, SizeType> view(IndexType from, IndexType to)
	{
		if (from == _size || from >= to)
			return {};

		_checkRange(from);
		_checkRange(to);
		return { _array + from, to - from };
	}

	constexpr void set(IndexType index, ConstSubscribedReferenceType value) requires std::copyable<SubscribedType>
	{
		_checkRange(index), _array[index] = value;
	}
	constexpr void set(IndexType index, SubscribedRValueType value) requires std::movable<SubscribedType>
	{
		_checkRange(index), _array[index] = std::move(value);
	}

	template <typename... _ArgsTys>
	constexpr void emplace(IndexType index, _ArgsTys&&... args) requires std::constructible_from<SubscribedType, _ArgsTys...>
	{
		_checkRange(index);
		std::destroy_at(_array + index);
		std::construct_at(_array + index, std::forward<_ArgsTys>(args)...);
	}

	constexpr ConstSubscribedReferenceType get(IndexType index) const { return _checkRange(index), _array[index]; }

	constexpr SubscribedReferenceType at(IndexType index) { return _checkRange(index), _array[index]; }
	constexpr ConstSubscribedReferenceType at(IndexType index) const { return _checkRange(index), _array[index]; }

	constexpr iterator begin() { return _array; }
	constexpr const_iterator begin() const { return _array; }
	constexpr const_iterator cbegin() const { return _array; }
	constexpr iterator end() { return _array + _size; }
	constexpr const_iterator end() const { return _array + _size; }
	constexpr const_iterator cend() const { return _array + _size; }

public:
	constexpr operator bool() const { return !empty(); }
	constexpr bool operator! () const { return empty(); }

	constexpr SubscribedReferenceType operator[] (IndexType index) { return _checkRange(index), _array[index]; }
	constexpr ConstSubscribedReferenceType operator[] (IndexType index) const { return _checkRange(index), _array[index]; }

private:
	constexpr void _destroy()
	{
		if (_array)
			delete[] _array;

		_array = nullptr;
		_size = 0;
	}

private:
	static constexpr ArrayType _newArray(SizeType size) { return new SubscribedType[size]; }

	static constexpr void _copydata(ArrayType dst, ConstArrayType src, SizeType len) requires std::copyable<SubscribedType>
	{
		for (SizeType i = 0; i < len; ++i)
			dst[i] = src[i];
	}

	static constexpr void _move(StaticArray& dst, StaticArray& src) noexcept
	{
		dst._array = src._array;
		dst._size = src._size;

		src._array = nullptr;
		src._size = 0;
	}

	[[noreturn]] constexpr void _checkRange(IndexType index) const
	{
		if (index < 0 || index >= _size)
			std::_Xout_of_range("StaticArray<T> out of range.");
	}

public:
	static constexpr void swap(StaticArray& left, StaticArray& right) noexcept
	{
		ArrayType auxArray = left._array;
		SizeType auxSize = left._size;

		left._array = right._array;
		left._size = right._size;
		right._array = auxArray;
		right._size = auxSize;
	}
};




template <typename _Ty>
using StaticMatrix = StaticArray<StaticArray<_Ty>>;
