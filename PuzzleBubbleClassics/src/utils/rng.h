#pragma once

#include <random>
#include <concepts>
#include <vector>


class RNG
{
public:
	using RandomGenerator = std::minstd_rand;
	using ResultType = RandomGenerator::result_type;
	using SeedType = RandomGenerator::result_type;

private:
	RandomGenerator _rand;
	ResultType _min;
	ResultType _max;

public:
	RNG(const RNG&) = default;
	RNG(RNG&&) noexcept = default;
	~RNG() = default;

	RNG& operator= (const RNG&) = default;
	RNG& operator= (RNG&&) noexcept = default;

public:
	inline RNG(SeedType seed, ResultType min, ResultType max) : _rand(seed), _min(std::min(min, max)), _max(std::max(min, max)) {}
	inline RNG(ResultType min, ResultType max) : _rand(), _min(std::min(min, max)), _max(std::max(min, max)) {}
	inline RNG(SeedType seed) : RNG(seed, RandomGenerator::min(), RandomGenerator::max()) {}
	inline RNG() : RNG(RandomGenerator::min(), RandomGenerator::max()) {}

	inline ResultType operator() (ResultType min, ResultType max)
	{
		const ResultType rmin = std::min(min, max);
		const ResultType rmax = std::max(min, max);
		return (_rand() % (rmax - rmin)) + rmin;
	}

	inline ResultType operator() (ResultType max) { return (*this)(_min, max); }
	inline ResultType operator() () { return (*this)(_min, _max); }

	inline float randomFloat()
	{
		const ResultType full = _max - _min;
		return static_cast<float>((_rand() % full) / full);
	}

	inline SeedType randomSeed(ResultType min, ResultType max) { return static_cast<SeedType>((*this)(min, max)); }
	inline SeedType randomSeed(ResultType max) { return static_cast<SeedType>((*this)( max)); }
	inline SeedType randomSeed() { return static_cast<SeedType>((*this)()); }

	inline RNG randomRNG(ResultType min, ResultType max) { return RNG((*this)(min, max)); }
	inline RNG randomRNG(ResultType max) { return RNG((*this)(max)); }
	inline RNG randomRNG() { return RNG((*this)()); }

	inline RNG& operator>> (ResultType& right) { return right = (*this)(), *this; }
	inline RNG& operator>> (float& right) { return right = randomFloat(), *this; }

public:
	inline bool operator== (std::integral auto right) { return static_cast<decltype(right)>((*this)()) == right; }
	inline bool operator== (std::floating_point auto right) { return static_cast<decltype(right)>(randomFloat()) == right; }

	inline std::strong_ordering operator<=> (std::integral auto right) { return static_cast<decltype(right)>((*this)()) <=> right; }
	inline std::partial_ordering operator<=> (std::floating_point auto right) { return static_cast<decltype(right)>(randomFloat()) <=> right; }

	friend inline bool operator== (std::integral auto left, RNG& right) { return right == left; }
	friend inline bool operator== (std::floating_point auto left, RNG& right) { return right == left; }

	friend inline std::strong_ordering operator<=> (std::integral auto left, RNG& right) { return left <=> static_cast<decltype(left)>(right()); }
	friend inline std::partial_ordering operator<=> (std::floating_point auto left, RNG& right) { return left <=> static_cast<decltype(left)>(right.randomFloat()); }

public:
	constexpr ResultType min() { return _min; }
	constexpr ResultType max() { return _max; }

public:
	template<typename _Ty, typename _ContainerTy> requires requires(_ContainerTy& cnt, std::size_t index)
	{
		{ cnt[index] } -> std::same_as<_Ty&>;
		{ cnt.size() } -> std::same_as<std::size_t>;
	}
	inline _Ty& randomEntry(_ContainerTy& container)
	{
		return container[std::size_t((*this)(container.size()))];
	}

	template<typename _Ty, typename _ContainerTy> requires requires(const _ContainerTy& cnt, std::size_t index)
	{
		{ cnt[index] } -> std::same_as<const _Ty&>;
		{ cnt.size() } -> std::same_as<std::size_t>;
	}
	inline const _Ty& randomEntry(const _ContainerTy& container)
	{
		return container[std::size_t((*this)(container.size()))];
	}

	template <typename _Ty>
	inline _Ty& randomEntry(std::vector<_Ty>& vector) { return vector[std::size_t((*this)(ResultType(vector.size())))]; }

	template <typename _Ty>
	inline const _Ty& randomEntry(const std::vector<_Ty>& vector) { return vector[std::size_t((*this)(ResultType(vector.size())))]; }
};
