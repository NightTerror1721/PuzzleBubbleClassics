#pragma once

#include <unordered_map>
#include <functional>
#include <concepts>
#include <memory>
#include <new>

template <typename _KeyTy, typename _Ty> requires
	std::default_initializable<_Ty> &&
	std::destructible<_Ty> &&
	std::default_initializable<std::hash<_KeyTy>> &&
	std::copyable<_KeyTy> &&
	requires(_KeyTy key) { { std::hash<_KeyTy>()(key) } -> std::same_as<std::size_t>; }
class SimpleContainer
{
private:
	struct Node
	{
		_KeyTy key;
		_Ty data;
		Node* next;
		Node* prev;
		SimpleContainer* owner;
	};

	Node* _front = nullptr;
	Node* _back = nullptr;
	size_t _size = 0;

	std::unordered_map<_KeyTy, Node*> _map;

private:
	Node* _create_node(const _KeyTy& key)
	{
		Node* node = new Node;
		node->owner = *this;
		node->key = key;
		return node;
	}

	Node* _insert_node(Node* node)
	{
		if (node->owner == this)
		{
			if (!_front)
			{
				_front = _back = node;
				node->next = node->prev = nullptr;
			}
			else
			{
				node->prev = _back;
				_back->next = node;
				_back = node;
			}
		}
		_size++;

		_map.insert(node->key, node);

		return node;
	}

	void _remove_node(Node* node)
	{
		if (node && node->owner == this)
		{
			_KeyTy key = node->key;
			if (node == _front)
			{
				if (_front == _back)
				{
					delete _front;
					_front = _back = nullptr;
				}
				else
				{
					_front = node->next;
					_front->prev = nullptr;
					delete node;
				}
			}
			else if (node == _back)
			{
				if (_front == _back)
				{
					delete _front;
					_front = _back = nullptr;
				}
				else
				{
					_back = node->prev;
					_front->next = nullptr;
					delete node;
				}
			}
			else
			{
				node->prev->next = node->next;
				node->next->prev = node->prev;
				delete node;
			}

			_size--;
			_map.erase(key);
		}
	}

	void _destroy()
	{
		if (_front)
		{
			for (Node *node = _front, *next; node; node = next)
			{
				next = node->next;
				delete node;
			}

			_front = _back = nullptr;
			_size = 0;
		}

		_map.clear();
	}

	SimpleContainer& _copy(const SimpleContainer& c, bool destroy) requires std::copy_constructible<_Ty>
	{
		if (destroy)
			_destroy();

		if (c->_front)
		{
			for (const Node* cnode = c->_front; cnode; cnode = cnode->next)
			{
				Node* node = _create_node(cnode->key);
				new (std::addressof(node->data)) _Ty{ cnode->data };
				_insert_node(node);
			}
		}

		_size = c->_size;

		return *this;
	}

	SimpleContainer& _move(SimpleContainer&& c, bool destroy)
	{
		if (destroy)
			_destroy();

		_front = c._front;
		_back = c._back;
		_size = c._size;
		_map = std::move(c._map);

		c._front = c._back = nullptr;
		c._size = 0;

		for (Node* node = _front; node; node = node->next)
			node->owner = this;

		return *this;
	}

public:
	inline SimpleContainer() = default;
	inline SimpleContainer(const SimpleContainer& other) requires std::copy_constructible<_Ty> : SimpleContainer() { _copy(other, false); }
	inline SimpleContainer(SimpleContainer&& other) noexcept : SimpleContainer() { _move(std::move(other), false); }
	inline ~SimpleContainer() { _destroy(); }

	inline SimpleContainer& operator= (const SimpleContainer& right) requires std::copy_constructible<_Ty> { return _copy(right, true); }
	inline SimpleContainer& operator= (SimpleContainer&& right) noexcept { return _move(std::move(right), true); }

	inline operator bool() const { return _front; }
	inline bool operator! () const { return !_front; }

public:
	using BucketRef = Node*;
	using ConstBucketRef = const Node*;
	static constexpr ConstBucketRef invalid_bucket = nullptr;

	inline bool empty() const { return _front; }
	inline size_t size() const { return _size; }


	inline _Ty* front() { return _front; }
	inline const _Ty* front() const { return _front; }
	inline _Ty* back() { return _back; }
	inline const _Ty* back() const { return _back; }

	inline bool exists(const _KeyTy& key) const { return _map.contains(key); }
	inline _Ty& get(const _KeyTy& key) { return _map.at(key); }
	inline const _Ty& get(const _KeyTy& key) const { return _map.at(key); }
	inline _Ty* find(const _KeyTy& key)
	{
		auto it = _map.find(key);
		if (it != _map.end())
			return it->second->data;
		return nullptr;
	}
	inline const _Ty* find(const _KeyTy& key) const
	{
		auto it = _map.find(key);
		if (it != _map.end())
			return it->second->data;
		return nullptr;
	}

	BucketRef insert(const _KeyTy& key, const _Ty& value) requires std::copy_constructible<_Ty>
	{
		Node* node = _create_node(key);
		new (std::addressof(node->data)) _Ty{ node->data };
		_insert_node(node);
		return node;
	}

	BucketRef insert(const _KeyTy& key, _Ty&& value) requires std::move_constructible<_Ty>
	{
		Node* node = _create_node(key);
		new (std::addressof(node->data)) _Ty{ std::move(node->data) };
		_insert_node(node);
		return node;
	}

	template <typename... _ArgsTys>
	BucketRef emplace(const _KeyTy& key, _ArgsTys&&...args) requires std::constructible_from<_Ty, _ArgsTys...>
	{
		Node* node = _create_node(key);
		new (std::addressof(node->data)) _Ty{ std::forward<_ArgsTys>(args)... };
		_insert_node(node);
		return node;
	}

	inline void erase(BucketRef ref)
	{
		_remove_node(ref);
	}

	inline void clear()
	{
		_destroy();
	}

	inline void erase(_KeyTy key)
	{
		auto it = _map.find(key);
		if (it != _map.end())
			_remove_node(it->second);
	}


public:
	class const_iterator;

	class iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = _Ty;
		using pointer = _Ty*;
		using reference = _Ty&;

	private:
		Node* _node;

	public:
		iterator(BucketRef ref) : _node(ref) {}

		reference operator* () const { return _node->data; }
		
		pointer operator-> () const { return std::addressof(_node->data); }

		iterator& operator++ ()
		{
			if (_node)
				_node = _node->next;
			return *this;
		}

		iterator operator++ (int)
		{
			iterator it = *this;
			++(*this);
			return it;
		}

		bool operator== (const iterator&) const = default;

		friend const_iterator;
	};

	class const_iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const _Ty;
		using pointer = const _Ty*;
		using reference = const _Ty&;

	private:
		const Node* _node;

	public:
		const_iterator(ConstBucketRef ref) : _node(ref) {}
		const_iterator(iterator it) : _node(it._node) {}

		reference operator* () const { return _node->data; }

		pointer operator-> () const { return std::addressof(_node->data); }

		const_iterator& operator++ ()
		{
			if (_node)
				_node = _node->next;
			return *this;
		}

		const_iterator operator++ (int)
		{
			const_iterator it = *this;
			++(*this);
			return it;
		}

		bool operator== (const const_iterator&) const = default;
	};

	inline iterator begin() { return _front; }
	inline const_iterator begin() const { return _front; }
	inline const_iterator cbegin() const { return _front; }
	inline iterator end() { return nullptr; }
	inline const_iterator end() const { return nullptr; }
	inline const_iterator cend() const { return nullptr; }

	iterator erase(iterator it)
	{
		if (!it->_node)
			return nullptr;
		
		Node* next = it->_node->next;
		_remove_node(it->_node);
		return next;
	}

public:
	void for_each(void (*action)(_Ty&))
	{
		for (Node* node = _front; node; node = node->next)
			action(node->data);
	}

	void for_each(void (*action)(const _Ty&)) const
	{
		for (const Node* node = _front; node; node = node->next)
			action(node->data);
	}

	void for_each(const std::function<void (_Ty&)>& action)
	{
		for (Node* node = _front; node; node = node->next)
			action(node->data);
	}

	void for_each(const std::function<void(const _Ty&)>& action) const
	{
		for (const Node* node = _front; node; node = node->next)
			action(node->data);
	}
};
