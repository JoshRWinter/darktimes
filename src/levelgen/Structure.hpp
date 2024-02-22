#pragma once

#include <vector>

#include <win/Win.hpp>
#include "LevelObjectsInternal.hpp"
#include "PropDefs.hpp"

struct StructureProp
{
	StructureProp(int floor_index, LevelSide side, float x, float y, const LevelPropDefinition &propdef)
		: floor_index(floor_index)
		, side(side)
		, x(x)
		, y(y)
		, propdef(propdef)
	{}

	int floor_index;
	LevelSide side;
	float x, y;
	LevelPropDefinition propdef;
};

struct StructureFloorConnection
{
	StructureFloorConnection(int x, int y)
		: index_x(x), index_y(y) {}

	int index_x;
	int index_y;
};
