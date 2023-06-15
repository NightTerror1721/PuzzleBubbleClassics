#pragma once

#include "utils/json.h"
#include "utils/transform_utils.h"

#include <compare>
#include <SFML/Graphics.hpp>


class GameObjectIdentifierFactory;

class GameObjectIdentifier
{
public:
	using IntegerType = Uint64;

	friend GameObjectIdentifierFactory;

private:
	static constexpr IntegerType InvalidId = 0;

private:
	IntegerType _id = InvalidId;

public:
	constexpr GameObjectIdentifier() noexcept = default;
	constexpr GameObjectIdentifier(const GameObjectIdentifier&) noexcept = default;
	constexpr GameObjectIdentifier(GameObjectIdentifier&&) noexcept = default;
	constexpr ~GameObjectIdentifier() noexcept = default;

	constexpr GameObjectIdentifier& operator= (const GameObjectIdentifier&) noexcept = default;
	constexpr GameObjectIdentifier& operator= (GameObjectIdentifier&&) noexcept = default;

	constexpr bool operator== (const GameObjectIdentifier&) const noexcept = default;
	constexpr auto operator<=> (const GameObjectIdentifier&) const noexcept = default;

private:
	constexpr explicit GameObjectIdentifier(IntegerType id) noexcept : _id(id) {}

public:
	constexpr operator bool() const noexcept { return _id != InvalidId; }
	constexpr bool operator! () const noexcept { return _id == InvalidId; }

	constexpr explicit operator IntegerType() const noexcept { return _id; }

	friend std::ostream& operator<< (std::ostream& left, GameObjectIdentifier right) { return left << right._id; }
	friend std::istream& operator>> (std::istream& left, GameObjectIdentifier& right) { return left >> right._id; }

public:
	friend std::hash<GameObjectIdentifier>;
};

namespace std
{
	template <>
	struct hash<GameObjectIdentifier> : public hash<GameObjectIdentifier::IntegerType>
	{
		inline std::size_t operator() (const GameObjectIdentifier& id) const noexcept
		{
			return hash<GameObjectIdentifier::IntegerType>::_Do_hash(id._id);
		}
	};
}


class GameObjectIdentifierFactory
{
private:
	GameObjectIdentifier::IntegerType _nextId = 1;

public:
	constexpr GameObjectIdentifierFactory() noexcept = default;
	constexpr GameObjectIdentifierFactory(const GameObjectIdentifierFactory&) noexcept = default;
	constexpr GameObjectIdentifierFactory(GameObjectIdentifierFactory&&) noexcept = default;
	constexpr ~GameObjectIdentifierFactory() noexcept = default;

	constexpr GameObjectIdentifierFactory& operator= (const GameObjectIdentifierFactory&) noexcept = default;
	constexpr GameObjectIdentifierFactory& operator= (GameObjectIdentifierFactory&&) noexcept = default;

public:

	constexpr GameObjectIdentifier operator() () noexcept { return GameObjectIdentifier(_nextId++); }
};


struct Renderable
{
	virtual void render(sf::RenderTarget& canvas, sf::RenderStates rs) = 0;
};

struct Updatable
{
	virtual void update(const sf::Time& elapsedTime) = 0;
};

struct EventDispatcher
{
	virtual void dispatchEvent(const sf::Event& event) = 0;
};


class GameObject : public virtual Renderable, public virtual Updatable, public virtual EventDispatcher
{
public:
	using Id = GameObjectIdentifier;

private:
	static inline GameObjectIdentifierFactory IdFactory = {};

private:
	Id _id = IdFactory();

public:
	GameObject() = default;
	GameObject(const GameObject&) = delete;
	GameObject(GameObject&&) noexcept = default;
	virtual ~GameObject() = default;

	GameObject& operator= (const GameObject&) = delete;
	GameObject& operator= (GameObject&&) noexcept = delete;

	constexpr bool operator== (const GameObject& right) const { return _id == right._id; }

public:
	constexpr Id getGameObjectId() const { return _id; }

	virtual inline void render(sf::RenderTarget& canvas, sf::RenderStates rs) override {}
	virtual constexpr void update(const sf::Time& elapsedTime) override {}
	virtual constexpr void dispatchEvent(const sf::Event& event) override {}
};


namespace utils
{
	template <std::derived_from<sf::Transformable> _DstTy, std::derived_from<sf::Transformable> _SrcTy>
	_DstTy& copyTransformableData(_DstTy& dst, const _SrcTy& src)
	{
		dst.setOrigin(src.getOrigin());
		dst.setPosition(src.getPosition());
		dst.setRotation(src.getRotation());
		dst.setScale(src.getScale());
	}
}
