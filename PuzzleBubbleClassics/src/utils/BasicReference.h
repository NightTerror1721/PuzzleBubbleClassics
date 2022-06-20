#pragma once

template <typename _Ty>
class BasicReference
{
public:
	using ValueType = _Ty;
	using ReferenceType = _Ty&;
	using PointerType = _Ty*;

private:
	PointerType _ref = nullptr;

public:
	BasicReference() = default;
	BasicReference(const BasicReference&) = default;
	BasicReference(BasicReference&&) noexcept = default;
	~BasicReference() = default;

	BasicReference& operator= (const BasicReference&) = default;
	BasicReference& operator= (BasicReference&&) noexcept = default;

	bool operator== (const BasicReference&) const = default;

public:
	inline bool empty() const { return !_ref; }

	inline BasicReference& reset(ReferenceType ref) { return _ref = ref, *this; }
	inline BasicReference& reset() { _ref = nullptr; }

	inline ReferenceType get() const { return *_ref; }

public:
	inline BasicReference& operator= (ReferenceType ref) { return _ref = std::addressof(ref), *this; }
	inline BasicReference& operator= (std::nullptr_t) { return _ref = nullptr, *this; }

	inline operator bool() const { return _ref; }
	inline bool operator! () const { return !_ref; }

	inline ReferenceType operator* () const { return *_ref; }
	inline PointerType operator-> () const { return _ref; }
};
