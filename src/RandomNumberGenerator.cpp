#include "RandomNumberGenerator.hpp"

RandomNumberGenerator::RandomNumberGenerator(int seed)
	: mersenne(seed)
{}

bool RandomNumberGenerator::one_in(int chances)
{
	return std::uniform_int_distribution<int>(1, chances)(mersenne) == 1;
}

int RandomNumberGenerator::uniform_int(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(mersenne);
}

float RandomNumberGenerator::uniform_real(float low, float high)
{
	return std::uniform_real_distribution<float>(low, high)(mersenne);
}
