#pragma once

#include <random>

#include "darktimes.hpp"

class RandomNumberGenerator
{
	NO_COPY_MOVE(RandomNumberGenerator);

public:
	RandomNumberGenerator(int);

	bool one_in(int);
	int uniform_int(int, int);
	float uniform_real(float, float);

	std::mt19937 mersenne;
};
