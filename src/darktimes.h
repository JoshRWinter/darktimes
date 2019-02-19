#ifndef DARKTIMES_H
#define DARKTIMES_H

#include <random>

#include <time.h>

#include <win.h>
#include "press.h"

#include "entity/entity.h"
#include "render/renderer.h"
#include "level.h"
#include "world.h"

#define dput(string, ...) press::fwriteln(stderr, string, ##__VA_ARGS__)

inline struct mersenne_random
{
	mersenne_random() : generator(time(NULL)) {}

	float operator()(float low, float high)
	{
		return std::uniform_real_distribution<float>(low, high)(generator);
	}

	int operator()(int low, int high)
	{
		return std::uniform_int_distribution<int>(low, high)(generator);
	}

	bool operator()(unsigned chance)
	{
		return std::uniform_int_distribution<int>(1, chance)(generator) == 1;
	}

	std::mt19937 generator;
} mersenne;

#endif
