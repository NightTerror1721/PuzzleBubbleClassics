#pragma once

#include <concepts>
#include <exception>

template <typename _Ty>
class ConstNullableReference;

class EmptyNullableReference : std::exception {};

template <typename _Ty>
class NullableReference
{
private:
	_Ty* _ptr = nullptr;

public:
	constexpr NullableReference() = default;
	constexpr NullableReference(const NullableReference&) = default;
	constexpr NullableReference(NullableReference&&) noexcept = default;
	constexpr ~NullableReference() = default;

	constexpr NullableReference& operator= (const NullableReference&) = default;
	constexpr NullableReference& operator= (NullableReference&&) noexcept = default;

	constexpr bool operator== (const NullableReference&) const = default;

public:
	constexpr NullableReference(_Ty& ref) : _ptr(std::addressof(ref)) {}

	constexpr bool empty() const { return !_ptr; }
	constexpr operator bool() const { return _ptr; }
	constexpr bool operator! () const { return !_ptr; }

	constexpr _Ty& get() const
	{
		if (_ptr)
			return *_ptr;
		throw EmptyNullableReference();
	}
	constexpr const _Ty& operator* () const { return get(); }

	constexpr _Ty* operator-> () const { return _ptr; }

	friend ConstNullableReference;
};


template <typename _Ty>
class ConstNullableReference
{
private:
	const _Ty* _ptr = nullptr;

public:
	constexpr ConstNullableReference() = default;
	constexpr ConstNullableReference(const ConstNullableReference&) = default;
	constexpr ConstNullableReference(ConstNullableReference&&) noexcept = default;
	constexpr ~ConstNullableReference() = default;

	constexpr ConstNullableReference& operator= (const ConstNullableReference&) = default;
	constexpr ConstNullableReference& operator= (ConstNullableReference&&) noexcept = default;

	constexpr bool operator== (const ConstNullableReference&) const = default;

public:
	constexpr ConstNullableReference(const _Ty& ref) : _ptr(std::addressof(ref)) {}
	constexpr ConstNullableReference(NullableReference<_Ty> ref) : _ptr(ref._ptr) {}

	constexpr bool empty() const { return !_ptr; }
	constexpr operator bool() const { return _ptr; }
	constexpr bool operator! () const { return !_ptr; }

	constexpr const _Ty& get() const
	{
		if (_ptr)
			return *_ptr;
		throw EmptyNullableReference();
	}
	constexpr const _Ty& operator* () const { return get(); }

	constexpr const _Ty* operator-> () const { return _ptr; }
};
