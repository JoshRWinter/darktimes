#ifndef RANDOM_NUMBER_GENERATOR_HPP
#define RANDOM_NUMBER_GENERATOR_HPP

#include <random>

#include "darktimes.hpp"

class RandomNumberGenerator
{
	NOCOPYMOVE(RandomNumberGenerator);

public:
	RandomNumberGenerator(int);

	bool one_in(int);
	int uniform_int(int, int);
	float uniform_real(float, float);

	std::mt19937 mersenne;
};

#endif