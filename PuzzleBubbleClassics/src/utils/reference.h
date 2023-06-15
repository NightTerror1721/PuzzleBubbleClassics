#pragma once

#include <concepts>
#include <compare>


template <typename _Ty>
class NullableReference
{
public:
	using ValueType = _Ty;
	using Reference = _Ty&;
	using Pointer = _Ty*;

private:
	Pointer _ptr = nullptr;

public:
	constexpr NullableReference() noexcept = default;
	constexpr NullableReference(const NullableReference&) noexcept = default;
	constexpr NullableReference(NullableReference&&) noexcept = default;
	constexpr ~NullableReference() noexcept = default;

	constexpr NullableReference& operator= (const NullableReference&) noexcept = default;
	constexpr NullableReference& operator= (NullableReference&&) noexcept = default;

	constexpr bool operator== (const NullableReference&) const noexcept = default;

public:
	constexpr NullableReference(decltype(nullptr)) noexcept : NullableReference() {}

	constexpr NullableReference(const NullableReference<std::remove_const_t<ValueType>>& ref) noexcept requires std::is_const_v<ValueType>
		: _ptr(ref._ptr)
	{}

	constexpr NullableReference(Pointer ptr) noexcept : _ptr(ptr) {}

public:
	constexpr bool operator== (decltype(nullptr)) const noexcept { return _ptr == nullptr; }
	constexpr bool operator!= (decltype(nullptr)) const noexcept { return _ptr != nullptr; }

	constexpr operator bool() const noexcept { return _ptr != nullptr; }
	constexpr bool operator! () const noexcept { return _ptr == nullptr; }

	constexpr Reference operator* () const noexcept { return *_ptr; }

	constexpr Pointer operator& () const noexcept { return _ptr; }

	constexpr Pointer operator-> () const noexcept { return _ptr; }

	template <typename _ArgTy> requires requires(ValueType& value, const _ArgTy& arg) { { value[arg] }; }
	constexpr decltype(auto) operator[] (const _ArgTy& arg) { return (*_ptr)[arg]; }

	template <typename _ArgTy> requires requires(ValueType& value, const _ArgTy& arg) { { value[arg] }; }
	constexpr decltype(auto) operator[] (const _ArgTy& arg) const { return (*_ptr)[arg]; }
};


template <typename _Ty>
using Reference = NullableReference<_Ty>;

template <typename _Ty>
using ConstReference = NullableReference<const _Ty>;
