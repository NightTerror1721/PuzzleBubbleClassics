#pragma once

#include "common.h"
#include "json.h"
#include "utils/SimpleContainer.h"

class GameObjectIdentifier
{
private:
	UInt64 _id = 0;

public:
	constexpr GameObjectIdentifier() = default;
	constexpr GameObjectIdentifier(const GameObjectIdentifier&) = default;
	constexpr GameObjectIdentifier(GameObjectIdentifier&&) noexcept = default;
	constexpr ~GameObjectIdentifier() = default;

	constexpr GameObjectIdentifier& operator= (const GameObjectIdentifier&) = default;
	constexpr GameObjectIdentifier& operator= (GameObjectIdentifier&&) noexcept = default;

	constexpr bool operator== (const GameObjectIdentifier&) const = default;
	constexpr auto operator<=> (const GameObjectIdentifier&) const = default;

	constexpr operator bool() const { return _id != 0; }
	constexpr bool operator! () const { return _id == 0; }

	constexpr explicit operator UInt64() const { return _id; }

public:
	Json serialize() const;
	void deserialize(const Json& json);

	static GameObjectIdentifier make();

	friend std::ostream& operator<< (std::ostream& left, const GameObjectIdentifier& right);
	friend std::istream& operator>> (std::istream& left, GameObjectIdentifier& right);
};

template <>
struct std::hash<GameObjectIdentifier> : std::hash<UInt64>
{
	inline [[nodiscard]] size_t operator() (GameObjectIdentifier goid) const
	{
		return (*static_cast<const std::hash<UInt64>*>(this))(static_cast<UInt64>(goid));
	}
};




struct Renderable
{
	virtual void render(sf::RenderTarget& canvas) = 0;
};

struct Updatable
{
	virtual void update(const sf::Time& delta) = 0;
};

struct EventDispatcher
{
	virtual void dispatchEvent(const sf::Event& event) = 0;
};




class GameObject : public Renderable, public Updatable, public EventDispatcher
{
private:
	GameObjectIdentifier _id = GameObjectIdentifier::make();

public:
	GameObject() = default;
	GameObject(const GameObject&) = default;
	GameObject(GameObject&&) noexcept = default;
	virtual ~GameObject() = default;

	GameObject& operator= (const GameObject&) = default;
	GameObject& operator= (GameObject&&) noexcept = default;

	bool operator== (const GameObject&) const = default;

public:
	inline GameObjectIdentifier getId() const { return _id; }

public:
	inline virtual void render(sf::RenderTarget& canvas) override {}
	inline virtual void update(const sf::Time& delta) override {}
	inline virtual void dispatchEvent(const sf::Event& event) override {}
};




template <std::derived_from<GameObject> _Ty>
class GameObjectContainer :
	SimpleContainer<GameObjectIdentifier, std::shared_ptr<_Ty>>,
	public Renderable,
	public Updatable,
	public EventDispatcher
{
private:
	using Container = typename SimpleContainer<GameObjectIdentifier, std::shared_ptr<_Ty>>;
	using BucketRef = typename Container::BucketRef;

public:
	GameObjectContainer() = default;
	GameObjectContainer(const GameObjectContainer&) = delete;
	GameObjectContainer(GameObjectContainer&&) noexcept = default;
	~GameObjectContainer() = default;

	GameObjectContainer& operator= (const GameObjectContainer&) = delete;
	GameObjectContainer& operator= (GameObjectContainer&&) noexcept = default;

private:
	inline std::shared_ptr<_Ty> _removeGameObject(GameObjectIdentifier id)
	{
		Container::erase(id);
	}

	std::shared_ptr<_Ty> _insertGameObject(std::shared_ptr<_Ty> ptr)
	{
		GameObjectIdentifier id = ptr->getId();
		_removeGameObject(id);

		Container::insert(id, ptr);

		return ptr;
	}

public:
	using iterator = Container::iterator;
	using const_iterator = Container::const_iterator;

	inline bool empty() const { return Container::empty(); }
	inline Size size() const { return Container::size(); }

	inline iterator begin() { return Container::begin(); }
	inline const_iterator begin() const { return Container::begin(); }
	inline const_iterator cbegin() const { return Container::cbegin(); }
	inline iterator end() { return Container::end(); }
	inline const_iterator end() const { return Container::end(); }
	inline const_iterator cend() const { return Container::cend(); }

public:
	template <typename _GOTy = _Ty, typename... _ArgsTys>
	std::shared_ptr<_Ty> createNewGameObject(_ArgsTys&&... args) requires
		std::constructible_from<_Ty, _ArgsTys...>&&
		std::derived_from<_GOTy, _Ty>
	{
		std::shared_ptr<_Ty> ptr = new _GOTy{ std::forward<_ArgsTys>(args)... };
		onCreate(*ptr);
		return _insertGameObject(ptr);
	}

	template <typename _GOTy = _Ty>
	inline std::shared_ptr<_Ty> insertGameObject(const std::shared_ptr<_Ty>& go) requires std::derived_from<_GOTy, _Ty>
	{
		_insertGameObject(go);
	}

	template <typename _GOTy = _Ty>
	inline std::shared_ptr<_Ty> insertGameObject(std::shared_ptr<_Ty>&& go) requires std::derived_from<_GOTy, _Ty>
	{
		_insertGameObject(std::move(go));
	}

	std::shared_ptr<_Ty> getGameObject(GameObjectIdentifier id)
	{
		std::shared_ptr<_Ty>* ptr = Container::find(id);
		if (ptr)
			return *ptr;
		return nullptr;
	}

	const std::shared_ptr<_Ty> getGameObject(GameObjectIdentifier id) const
	{
		const std::shared_ptr<_Ty>* ptr = Container::find(id);
		if (ptr)
			return *ptr;
		return nullptr;
	}

	const std::weak_ptr<_Ty> getWeakGameObject(GameObjectIdentifier id) const
	{
		const std::shared_ptr<_Ty>* ptr = Container::find(id);
		if (ptr)
			return *ptr;
		return nullptr;
	}

	std::weak_ptr<_Ty> getWeakGameObject(GameObjectIdentifier id)
	{
		std::shared_ptr<_Ty>* ptr = Container::find(id);
		if (ptr)
			return *ptr;
		return nullptr; 
	}

public:
	void render(sf::RenderTarget& canvas) override
	{
		for (auto& ptr : *this)
			ptr->render(canvas);
	}

	void update(const sf::Time& delta) override
	{
		for (auto& ptr : *this)
			ptr->update(delta);
	}

	void dispatchEvent(const sf::Event& event) override
	{
		for (auto& ptr : *this)
			ptr->dispatchEvent(event);
	}

public:
	inline virtual void onCreate(GameObject& element) {};
};
