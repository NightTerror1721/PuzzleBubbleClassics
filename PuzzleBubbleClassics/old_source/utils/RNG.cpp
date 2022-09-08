#include "RNG.h"

#include <chrono>

RNG::RNG(RandomValue min, RandomValue max) :
	RNG(static_cast<Seed>(std::chrono::system_clock::now().time_since_epoch().count()), min, max)
{}

RNG::RNG(Seed seed, RandomValue min, RandomValue max) :
	_rand(seed),
	_min(std::min(min, max)),
	_max(std::max(min, max))
{}

RNG::RandomValue RNG::operator() (RandomValue __min, RandomValue __max)
{
	auto min = std::min(__min, __max);
	auto max = std::max(__min, __max);
	return (_rand() % (max - min)) + min;
}

RNG::RandomValue RNG::operator() (RandomValue __max)
{
	auto min = std::min(_min, __max);
	auto max = std::max(_min, __max);
	return (_rand() % (max - min)) + min;
}

float RNG::randomFloat()
{
	auto full = _max - _min;
	return static_cast<float>((_rand() % full)) / full;
}
