#pragma once

#include <random>

#include "Darktimes.hpp"

class RandomNumberGenerator
{
	NO_COPY_MOVE(RandomNumberGenerator);

public:
	explicit RandomNumberGenerator(int);

	bool one_in(int);
	int uniform_int(int, int);
	float uniform_real(float, float);

	std::mt19937 mersenne;
};
