#pragma once

#include <string>
#include <memory>
#include <concepts>
#include <exception>
#include <unordered_map>

import Utils.NullableReference;

template <typename _Ty, typename _KeyTy = std::string> requires
	std::default_initializable<_Ty>&&
	std::destructible<_Ty>&&
	std::default_initializable<std::hash<_KeyTy>>&&
	std::copyable<_KeyTy>&&
	requires(_KeyTy key) { { std::hash<_KeyTy>()(key) } -> std::same_as<std::size_t>; }
class SimpleElementManager
{
private:
	SimpleElementManager<_Ty, _KeyTy>* _parent = nullptr;
	std::unordered_map<_KeyTy, std::unique_ptr<_Ty>> _map;

public:
	SimpleElementManager() = default;
	SimpleElementManager(const SimpleElementManager&) = delete;
	SimpleElementManager(SimpleElementManager&&) noexcept = default;
	~SimpleElementManager() = default;

	SimpleElementManager& operator= (const SimpleElementManager&) = delete;
	SimpleElementManager& operator= (SimpleElementManager&&) noexcept = default;

public:
	SimpleElementManager(SimpleElementManager& parent) : _parent(std::addressof(parent)) {}

private:
	bool _destroyElement(const _KeyTy& key)
	{
		auto it = _map.find(key);
		if (it != _map.end())
		{
			it->second.reset(nullptr);
			_map.erase(it);
			return true;
		}

		return false;
	}

protected:
	template <typename _ElementTy = _Ty, typename... _ArgsTys>
	_Ty* create(const _KeyTy& key, _ArgsTys&&... args) requires
		std::derived_from<_ElementTy, _Ty> &&
		std::constructible_from<_ElementTy, _ArgsTys...>
	{
		_destroyElement(key);

		auto res = _map.emplace(std::piecewise_construct,
			std::make_tuple(key),
			std::make_tuple(new _ElementTy{ std::forward<_ArgsTys>(args)... })
		);

		if (res.second)
			return res.first->second.get();
		return nullptr;
	}

public:
	inline bool empty() const { return _map.empty(); }
	inline std::size_t size() const { return _map.size(); }

	inline bool hasParent() const { return _parent; }
	inline SimpleElementManager<_Ty, _KeyTy>& parent() { return *_parent; }
	inline const SimpleElementManager<_Ty, _KeyTy>& parent() const { return *_parent; }

	inline bool exists(const _KeyTy& key) { return _map.contains(key); }

	NullableReference<_Ty> opt(const _KeyTy& key)
	{
		auto it = _map.find(key);
		if (it != _map.end())
			return *it->second;
		if (_parent)
			return _parent->get(key);
		return {};
	}

	ConstNullableReference<_Ty> opt(const _KeyTy& key) const
	{
		auto it = _map.find(key);
		if (it != _map.end())
			return *it->second;
		if (_parent)
			return _parent->get(key);
		return {};
	}

	inline _Ty& get(const _KeyTy& key)
	{
		auto ref = opt(key);
		if (!ref.empty())
			return ref.get();
		throw NoSuchElement(key);
	}

	const _Ty& get(const _KeyTy& key) const
	{
		auto ref = opt(key);
		if (ref)
			return *ref;
		throw NoSuchElement(key);
	}

	inline void destroy(const _KeyTy& key)
	{
		_destroyElement(key);
	}

	inline void clear()
	{
		_map.clear();
	}
	

public:
	inline _Ty& operator[] (const _KeyTy& key) { return get(key); }
	inline const _Ty& operator[] (const _KeyTy& key) const { return get(key); }

public:
	class NoSuchElement : public std::exception
	{
		friend SimpleElementManager;

		static std::string make_msg(const _KeyTy& key)
		{
			std::string keytext;
			if constexpr (std::same_as<_KeyTy, std::string>)
				keytext = key;
			else if constexpr (requires(const _KeyTy & k) { { std::to_string(k) }; })
				keytext = std::to_string(key);
			else
				keytext = "<<last provided key>>";

			return keytext + " key not found.";
		}

		inline NoSuchElement(const _KeyTy& key) : exception(make_msg(key).c_str()) {}
	};
};
