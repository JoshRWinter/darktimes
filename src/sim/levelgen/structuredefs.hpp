#pragma once

#include "structure.hpp"
#include "propdefs.hpp"

inline Structure small1
{
	std::vector<StructureFloor>
	{
		StructureFloor(0, 0.0f, 0.0f, 4.0f, 2.0f, true, {}),

		StructureFloor(0, -1.0f, 2.0f, 6.0f, 2.0f, false, { StructureProp(LevelPropOrientation::up, 0.1f, 0.1f, PropDefinitions::side_tables.at(0)) })
	},

	std::vector<StructureFloorConnection>
	{
		StructureFloorConnection(0, 1)
	}
};

inline Structure small2
{
	std::vector<StructureFloor>
	{
		StructureFloor(0, 0.0f, 0.0f, 3.6f, 6.0f, false, {}),
		StructureFloor(0, 3.6f, 1.5f, 4.0f, 3.0f, false, {}),
		StructureFloor(0, -2.0f, 6.0f, 7.0f, 3.0f, false, {})
	},

	std::vector<StructureFloorConnection>
	{
		StructureFloorConnection(0, 1),
		StructureFloorConnection(0, 2)
	}
};

inline Structure small3
{
	std::vector<StructureFloor>
	{
		StructureFloor(0, 0.0f, 0.0f, 8.0f, 2.2f, false, {}),
		StructureFloor(0, 4.9f, 2.2f, 4.0f, 7.4f, false, {}),
		StructureFloor(0, -3.0f, -0.5f, 3.0f, 6.2f, false, {}),
		StructureFloor(0, -2.5f, 5.7f, 7.4f, 2.2f, false, {})
	},

	std::vector<StructureFloorConnection>
	{
		StructureFloorConnection(0, 1),
		StructureFloorConnection(0, 2),
		StructureFloorConnection(2, 3),
		StructureFloorConnection(1, 3),
	}
};


inline std::vector<Structure> structure_defs
{
	small1,
	small2,
	small3
};
