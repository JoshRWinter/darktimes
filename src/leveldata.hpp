#pragma once

#include <vector>
#include "sim/levelgen/levelobjects.hpp"

struct LevelData
{
	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;
	std::vector<LevelProp> props;
	int seed;
};
