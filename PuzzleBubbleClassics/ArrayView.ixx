module;

export module Utils.Arrays:ArrayView;

import <concepts>;
import <vector>;
import <array>;
import <string>;
import <new>;

import :ArrayIterator;

export
template<typename _Ty, std::unsigned_integral _SizeTy = std::size_t>
class ArrayView
{
public:
	using SubscribedType = _Ty;
	using ConstSubscribedType = const _Ty;
	using SubscribedTypeReference = _Ty&;
	using ConstSubscribedTypeReference = const _Ty&;
	using SubscribedTypeRValue = _Ty&&;
	using ArrayType = _Ty*;
	using SizeType = _SizeTy;
	using IndexType = _SizeTy;
	using iterator = ArrayIterator<SubscribedType>;
	using const_iterator = ConstArrayIterator<SubscribedType>;

private:
	SizeType _size = 0;
	ArrayType _array = nullptr;

public:
	constexpr ArrayView() = default;
	constexpr ArrayView(const ArrayView&) = default;
	constexpr ArrayView(ArrayView&&) noexcept = default;
	constexpr ~ArrayView() = default;

	constexpr ArrayView& operator= (const ArrayView&) = default;
	constexpr ArrayView& operator= (ArrayView&&) noexcept = default;

	constexpr bool operator== (const ArrayView&) const = default;

public:
	constexpr ArrayView(ArrayType array, SizeType size) : _size(size), _array(array) {}
	constexpr ArrayView(std::vector<SubscribedType>& v) : _size(v.size()), _array(v.data()) {}

	template <std::size_t _Size>
	constexpr ArrayView(std::array<SubscribedType, _Size>& v) : _size(v.size()), _array(v.data()) {}

	constexpr SizeType size() const { return _size; }
	constexpr bool empty() const { return _size == 0 || !_array; }

	constexpr void set(IndexType index, ConstSubscribedTypeReference value) requires std::copyable<SubscribedType>
	{
		check_range(index), _array[index] = value;
	}
	constexpr void set(IndexType index, SubscribedTypeRValue value) requires std::movable<SubscribedType>
	{
		check_range(index), _array[index] = std::move(value);
	}

	template <typename... _ArgsTys>
	constexpr void emplace(IndexType index, _ArgsTys&&... args) requires
		std::constructible_from<SubscribedType, _ArgsTys...>&&
		std::destructible<SubscribedType>
	{
		check_range(index);
		std::destroy_at(_array + index);
		std::construct_at(_array + index, std::forward<_ArgsTys>(args)...);
	}

	constexpr SubscribedTypeReference at(IndexType index) { return check_range(index), _array[index]; }
	constexpr ConstSubscribedTypeReference at(IndexType index) const { return check_range(index), _array[index]; }

	constexpr iterator begin() { return _array; }
	constexpr const_iterator begin() const { return _array; }
	constexpr const_iterator cbegin() const { return _array; }
	constexpr iterator end() { return _array + _size; }
	constexpr const_iterator end() const { return _array + _size; }
	constexpr const_iterator cend() const { return _array + _size; }

public:
	constexpr operator bool() const { return !empty(); }
	constexpr bool operator! () const { return empty(); }

	constexpr SubscribedTypeReference operator[] (IndexType index) { return check_range(index), _array[index]; }
	constexpr ConstSubscribedTypeReference operator[] (IndexType index) const { return check_range(index), _array[index]; }


private:
	[[noreturn]] constexpr void check_range(IndexType index)
	{
		if (index < 0 || index >= _size)
			std::_Xout_of_range("DynamicArrayView<T> out of range.");
	}
};
