#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <vector>

#include <win/win.hpp>
#include "levelobjects.hpp"

void correct_structure_properties(const LevelSide side, const float origin_x, const float origin_y, const float source_x, const float source_y, const float source_width, const float source_height, float &out_x, float &out_y, float &out_width, float &out_height);

struct StructureFloor
{
	StructureFloor(int texture, float x, float y, float width, float height)
		: texture(texture), x(x), y(y), width(width), height(height) {}

	LevelFloor get_floor(LevelSide side, float origin_x, float origin_y) const
	{
		float out_x, out_y, out_width, out_height;

		correct_structure_properties(side, origin_x, origin_y, x, y, width, height, out_x, out_y, out_width, out_height);

		return LevelFloor(texture, out_x, out_y, out_width, out_height);
	}

	int texture;
	float x, y;
	float width, height;
};

struct StructureFloorConnection
{
	StructureFloorConnection(int x, int y)
		: index_x(x), index_y(y) {}

	int index_x;
	int index_y;
};

struct StructureProp
{
	float x, y;
	LevelPropDefinition propdef;
};

struct Structure
{
	std::vector<StructureFloor> floors;
	std::vector<StructureFloorConnection> connections;
	std::vector<StructureProp> props;
};

#endif
