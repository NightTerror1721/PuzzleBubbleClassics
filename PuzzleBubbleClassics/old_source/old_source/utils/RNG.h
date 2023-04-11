#pragma once

#include <random>


class RNG
{
public:
	typedef unsigned int RandomValue;
	typedef unsigned int Seed;

private:
	std::minstd_rand _rand;
	RandomValue _min;
	RandomValue _max;

public:
	RNG(const RNG&) = default;
	RNG(RNG&&) noexcept = default;
	~RNG() = default;

	RNG& operator= (const RNG&) = default;
	RNG& operator= (RNG&&) noexcept = default;

public:
	RNG(RandomValue min, RandomValue max);
	RNG(Seed seed, RandomValue min, RandomValue max);

	RandomValue operator() (RandomValue min, RandomValue max);
	RandomValue operator() (RandomValue max);

	float randomFloat();

public:
	static constexpr RandomValue minRandomValue() { return std::minstd_rand::min(); }
	static constexpr RandomValue maxRandomValue() { return std::minstd_rand::max(); }

	inline RNG() : RNG(0, maxRandomValue()) {}
	inline RNG(Seed seed) : RNG(seed, 0, maxRandomValue()) {}

	inline RandomValue min() const { return _min; }
	inline RandomValue max() const { return _max; }

	inline RandomValue operator() () { return (_rand() % (_max - _min)) + _min; }

	inline Seed randomSeed(RandomValue min, RandomValue max) { return static_cast<Seed>((*this)(min, max)); }
	inline Seed randomSeed(RandomValue max) { return static_cast<Seed>((*this)(max)); }
	inline Seed randomSeed() { return static_cast<Seed>((*this)()); }

	inline RNG randomRNG(RandomValue min, RandomValue max) { return RNG(randomSeed(min, max)); }
	inline RNG randomRNG(RandomValue max) { return RNG(randomSeed(max)); }
	inline RNG randomRNG() { return RNG(randomSeed()); }

	inline RNG& operator>> (RandomValue& right) { return right = (*this)(), *this; }
	inline RNG& operator>> (float& right) { return right = randomFloat(), *this; }

	inline bool operator== (std::integral auto right) { return (*this)() == right; }
	inline bool operator== (std::floating_point auto right) { return randomFloat() == right; }

	inline bool operator!= (std::integral auto right) { return (*this)() != right; }
	inline bool operator!= (std::floating_point auto right) { return randomFloat() != right; }

	inline bool operator> (std::integral auto right) { return (*this)() > right; }
	inline bool operator> (std::floating_point auto right) { return randomFloat() > right; }

	inline bool operator< (std::integral auto right) { return (*this)() < right; }
	inline bool operator< (std::floating_point auto right) { return randomFloat() < right; }

	inline bool operator>= (std::integral auto right) { return (*this)() >= right; }
	inline bool operator>= (std::floating_point auto right) { return randomFloat() >= right; }

	inline bool operator<= (std::integral auto right) { return (*this)() <= right; }
	inline bool operator<= (std::floating_point auto right) { return randomFloat() <= right; }

	inline friend bool operator> (std::integral auto left, RNG& right) { return right <= left; }
	inline friend bool operator> (std::floating_point auto left, RNG& right) { return right <= left; }

	inline friend bool operator< (std::integral auto left, RNG& right) { return right >= left; }
	inline friend bool operator< (std::floating_point auto left, RNG& right) { return right >= left; }

	inline friend bool operator>= (std::integral auto left, RNG& right) { return right < left; }
	inline friend bool operator>= (std::floating_point auto left, RNG& right) { return right < left; }

	inline friend bool operator<= (std::integral auto left, RNG& right) { return right > left; }
	inline friend bool operator<= (std::floating_point auto left, RNG& right) { return right > left; }
};





template <RNG::RandomValue _DiceSize>
class Dice : public RNG
{
public:
	static constexpr RNG::RandomValue size = _DiceSize;

private:
	inline Dice() : RNG(0, size) {}
	inline Dice(Seed seed) : RNG(seed, 0, size) {}
};

using D2 = Dice<2>;
using D6 = Dice<6>;
using D10 = Dice<10>;
using D16 = Dice<16>;
using D20 = Dice<20>;
using D32 = Dice<32>;
using D50 = Dice<50>;
using D64 = Dice<64>;
using D100 = Dice<100>;
using D128 = Dice<128>;
using D256 = Dice<256>;
using D512 = Dice<512>;
using D1024 = Dice<1024>;
using D2048 = Dice<2048>;
using D4096 = Dice<4096>;
using D8192 = Dice<8192>;
using D65536 = Dice<65536>;
