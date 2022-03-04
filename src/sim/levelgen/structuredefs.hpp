#ifndef STRUCTURE_DEFS_HPP
#define STRUCTURE_DEFS_HPP

#include "structure.hpp"
#include "propdefs.hpp"

inline Structure test
{
    std::vector<StructureFloor>
	{
		StructureFloor(0, 0.0f, 0.0f, 4.0f, 2.0f, true, {}),

		StructureFloor(0, -1.0f, 2.0f, 6.0f, 2.0f, false, { StructureProp(LevelPropOrientation::up, 0.1f, 0.1f, PropDefinitions::side_tables.at(0)) })
	},

	std::vector<StructureFloorConnection>
	{
		StructureFloorConnection(0, 1)
	},
};

inline std::vector<Structure> structure_defs
{
	test
};

#endif
