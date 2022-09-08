module;

#include "headers.h"

export module Globals;

import Common;

export class GlobalComponent
{
public:
	virtual void init() = 0;
	virtual void destroy() = 0;
};

export
template <std::destructible _Ty>
class AbstractAllocatorGlobalComponent : public GlobalComponent
{
public:
	virtual inline _Ty& data() = 0;
};

export
template <typename _Ty, typename _DummyTy> requires
	std::constructible_from<_Ty, _DummyTy> &&
	std::destructible<_Ty> &&
	std::copyable<_DummyTy>
class AllocatorGlobalComponentWithDummy : public AbstractAllocatorGlobalComponent<_Ty>
{
private:
	_Ty* _data;
	_DummyTy* _dummy;

public:
	AllocatorGlobalComponentWithDummy(_DummyTy dummy_value) :
		_dummy(new _DummyTy{ dummy_value }),
		_data(nullptr)
	{}

	virtual void init() override
	{
		if (_data == nullptr && _dummy != nullptr)
		{
			_data = new _Ty{ *_dummy };
			delete _dummy;
			_dummy = nullptr;
		}
	}

	virtual void destroy() override
	{
		if (_data != nullptr)
		{
			delete _data;
			_data = nullptr;
		}
	}

	inline _Ty& data() override { return *_data; }
};

export
template <typename _Ty> requires
	std::default_initializable<_Ty> &&
	std::destructible<_Ty>
class AllocatorGlobalComponent : public AbstractAllocatorGlobalComponent<_Ty>
{
private:
	_Ty* _data;

public:
	AllocatorGlobalComponent() :
		_data(nullptr)
	{}

	virtual void init() override
	{
		if (_data == nullptr)
			_data = new _Ty;
	}

	virtual void destroy() override
	{
		if (_data != nullptr)
		{
			delete _data;
			_data = nullptr;
		}
	}

	inline _Ty& data() override { return *_data; }
};



namespace globals
{
	class GlobalElementContainer
	{
		std::vector<GlobalComponent*> _priority;
		std::vector<GlobalComponent*> _normal;
		bool _initiated = false;

	public:
		GlobalElementContainer() = default;
		GlobalElementContainer(const GlobalElementContainer&) = delete;
		GlobalElementContainer(GlobalElementContainer&&) noexcept = delete;

		GlobalElementContainer& operator= (const GlobalElementContainer&) = delete;
		GlobalElementContainer& operator= (GlobalElementContainer&&) noexcept = delete;

	public:
		~GlobalElementContainer()
		{
			if (_initiated)
			{
				for (auto* elem : _normal)
				{
					elem->destroy();
					delete elem;
				}
				_normal.clear();

				for (auto* elem : std::ranges::reverse_view(_priority))
				{
					if (elem != nullptr)
					{
						elem->destroy();
						delete elem;
					}
				}
				_priority.clear();
			}
		}

		void init()
		{
			if (!_initiated)
			{
				_initiated = true;

				for (auto* elem : _priority)
					elem->init();
				for (auto* elem : _normal)
					elem->init();
			}
		}

		Size registerPriorityElement(Size level, GlobalComponent* elem)
		{
			if (_priority.size() < level + 1)
			{
				_priority.resize(level + 1);
			}

			if (_priority[level] != nullptr)
				throw std::exception((std::string("Global Element with " + std::to_string(level) + " priority already defined.")).c_str());

			if (elem == nullptr)
				throw std::exception((std::string("Cannot build Global Element at " + std::to_string(level) + " priority.")).c_str());

			_priority[level] = elem;

			return level;
		}

		Size registerElement(GlobalComponent* elem)
		{
			if (elem == nullptr)
				throw std::exception("Cannot build Global Element.");

			Size index = static_cast<Size>(_normal.size());
			_normal.push_back(elem);

			return index;
		}


		template <std::derived_from<GlobalComponent> _Ty>
		inline _Ty& getPriorityElement(Size index)
		{
			return *reinterpret_cast<_Ty*>(_priority[index]);
		}

		template <typename _Ty>
		inline _Ty& getPriorityElementData(Size index)
		{
			return reinterpret_cast<AbstractAllocatorGlobalComponent<_Ty>*>(_priority[index])->data();
		}

		template <std::derived_from<GlobalComponent> _Ty>
		inline _Ty& getElement(Size index)
		{
			return *reinterpret_cast<_Ty*>(_normal[index]);
		}

		template <typename _Ty>
		inline _Ty& getElementData(Size index)
		{
			return reinterpret_cast<AbstractAllocatorGlobalComponent<_Ty>*>(_normal[index])->data();
		}

	public:
		static GlobalElementContainer elements;
	};
	
}



namespace globals
{
	export using PriorityLevel = Size;
	export using PriorityElementIndex = PriorityLevel;
	export using ElementIndex = Size;

	export inline PriorityElementIndex registerPriorityElement(PriorityLevel level, GlobalComponent* elem)
	{
		return GlobalElementContainer::elements.registerPriorityElement(level, elem);
	}

	export
	template <typename _Ty>
	inline PriorityElementIndex createPriorityElement(PriorityLevel level)
	{
		return GlobalElementContainer::elements.registerPriorityElement(level, new AllocatorGlobalComponent<_Ty>());
	}

	export
	template <typename _Ty, typename _DummyTy>
	inline PriorityElementIndex createPriorityElement(PriorityLevel level, _DummyTy dummy_value)
	{
		return GlobalElementContainer::elements.registerPriorityElement(level, new AllocatorGlobalComponentWithDummy<_Ty, _DummyTy>(dummy_value));
	}


	export inline ElementIndex registerElement(GlobalComponent* elem)
	{
		return GlobalElementContainer::elements.registerElement(elem);
	}

	export
	template <std::derived_from<GlobalComponent> _Ty>
	inline _Ty& getPriorityElement(PriorityElementIndex index)
	{
		return GlobalElementContainer::elements.getPriorityElement<_Ty>(index);
	}

	export
	template <typename _Ty>
	inline _Ty& getPriorityElementData(PriorityElementIndex index)
	{
		return GlobalElementContainer::elements.getPriorityElementData<_Ty>(index);
	}

	export
	template <std::derived_from<GlobalComponent> _Ty>
	inline _Ty& getElement(ElementIndex index)
	{
		return GlobalElementContainer::elements.getElement<_Ty>(index);
	}

	export
	template <typename _Ty>
	inline _Ty& getElementData(ElementIndex index)
	{
		return GlobalElementContainer::elements.getElementData<_Ty>(index);
	}



	export void initGlobals()
	{
		GlobalElementContainer::elements.init();
	}
}


module :private;

globals::GlobalElementContainer globals::GlobalElementContainer::elements;
