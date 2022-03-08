#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <vector>

#include <win/win.hpp>
#include "levelobjects.hpp"

void correct_structure_properties(const LevelSide side, const float origin_x, const float origin_y, const float source_x, const float source_y, const float source_width, const float source_height, float &out_x, float &out_y, float &out_width, float &out_height);

struct StructureProp
{
	StructureProp(LevelPropOrientation orientation, float x, float y, const LevelPropDefinition &propdef)
		: orientation(orientation), x(x), y(y), propdef(propdef) {}

	LevelPropOrientation orientation;
	float x, y;
	LevelPropDefinition propdef;
};

struct StructureFloor
{
	StructureFloor(int texture, float x, float y, float width, float height, bool skip_prop_generation, const std::vector<StructureProp> &props)
		: texture(texture), x(x), y(y), width(width), height(height), skip_prop_generation(skip_prop_generation), prop_spawns(props) {}

	LevelFloor get_floor(LevelSide side, float origin_x, float origin_y) const
	{
		float out_x, out_y, out_width, out_height;

		correct_structure_properties(side, origin_x, origin_y, x, y, width, height, out_x, out_y, out_width, out_height);

		LevelFloor floor(texture, out_x, out_y, out_width, out_height, skip_prop_generation);

		for (const StructureProp &prop : prop_spawns)
		{
			const float absolute_x = x + prop.x;
			const float absolute_y = y + prop.y;

			float out_prop_x, out_prop_y, out_prop_width, out_prop_height;
			correct_structure_properties(side, origin_x, origin_y, absolute_x, absolute_y, prop.propdef.get_width(prop.orientation), prop.propdef.get_height(prop.orientation), out_prop_x, out_prop_y, out_prop_width, out_prop_height);

			LevelPropOrientation orientations[] = { LevelPropOrientation::up, LevelPropOrientation::right, LevelPropOrientation::down, LevelPropOrientation::left };
			int increment;
			switch (side)
			{
				case LevelSide::top: increment = 0; break;
				case LevelSide::right: increment = 1; break;
				case LevelSide::bottom: increment = 2; break;
				case LevelSide::left: increment = 3; break;
			}

			const LevelPropOrientation new_orientation = orientations[increment % 4];

			floor.prop_spawns.push_back(LevelProp(new_orientation, prop.propdef, out_prop_x, out_prop_y));
		}

		return floor;
	}

	int texture;
	float x, y;
	float width, height;
	bool skip_prop_generation;
	std::vector<StructureProp> prop_spawns;
};

struct StructureFloorConnection
{
	StructureFloorConnection(int x, int y)
		: index_x(x), index_y(y) {}

	int index_x;
	int index_y;
};

struct Structure
{
	std::vector<StructureFloor> floors;
	std::vector<StructureFloorConnection> connections;
};

#endif
