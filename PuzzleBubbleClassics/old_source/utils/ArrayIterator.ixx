export module Utils.Arrays:ArrayIterator;

import <iterator>;
import <concepts>;


export
template <typename _Ty>
class ArrayIterator;

export
template <typename _Ty>
using ConstArrayIterator = ArrayIterator<const _Ty>;


export
template <typename _Ty>
class ArrayIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = _Ty;
    using pointer = _Ty*;
    using reference = _Ty&;

private:
    pointer _ptr;

public:
    constexpr ArrayIterator(pointer ptr) : _ptr(ptr) {}
    constexpr ArrayIterator(const ArrayIterator&) = default;
    constexpr ArrayIterator(ArrayIterator&&) noexcept = default;
    constexpr ~ArrayIterator() = default;

    constexpr ArrayIterator& operator= (const ArrayIterator&) = default;
    constexpr ArrayIterator& operator= (ArrayIterator&&) noexcept = default;

    constexpr bool operator== (const ArrayIterator&) const = default;
    constexpr auto operator<=> (const ArrayIterator&) const = default;

public:
    constexpr reference operator* () const { return *_ptr; }

    constexpr pointer operator-> () { return _ptr; }

    constexpr ArrayIterator& operator++ () { return _ptr++, * this; }
    constexpr ArrayIterator operator++ (int) { ArrayIterator tmp(*this); return ++(*this), tmp; }

public:
    template <std::same_as<const _Ty> _Ty2>
    constexpr operator ArrayIterator<_Ty2>() const { return ArrayIterator<_Ty2>(_ptr); }
};
