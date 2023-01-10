#ifndef LEVEL_DATA_SYNC_OBJECT_HPP
#define LEVEL_DATA_SYNC_OBJECT_HPP

#include <vector>
#include "sim/levelgen/levelobjects.hpp"

struct LevelDataSyncObject
{
	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;
	std::vector<LevelProp> props;
	int seed;
};

#endif
