#pragma once

#include "Structure.hpp"
#include "PropDefs.hpp"

struct StructureDefinition
{
	std::vector<LevelFloorInternal> floors;
	std::vector<StructureFloorConnection> connections;
	std::vector<StructureProp> props;

	std::vector<LevelFloorInternal> spawn(const std::vector<LevelFloorInternal> &existing, const LevelFloor &start_floor, LevelSide side) const
	{
		// crude validation
		if (floors.empty())
			win::bug("Structure no floors");
		if (floors.at(0).x != 0.0f || floors.at(0).y != 0.0f)
			win::bug("Structure first floor must be at (0, 0)");

		float start_x, start_y;

		switch (side)
		{
			case LevelSide::left:
				start_x = start_floor.x;
				start_y = (start_floor.y + (start_floor.h / 2.0f)) + (floors.at(0).y + (floors.at(0).h / 2.0f));
				break;
			case LevelSide::right:
				start_x = start_floor.x + start_floor.w;
				start_y = (start_floor.y + (start_floor.h / 2.0f)) - (floors.at(0).y + (floors.at(0).h / 2.0f));
				break;
			case LevelSide::bottom:
				start_x = (start_floor.x + (start_floor.w / 2.0f)) - (floors.at(0).y + (floors.at(0).h / 2.0f));
				start_y = start_floor.y;
				break;
			case LevelSide::top:
				start_x = (start_floor.x + (start_floor.w / 2.0f)) + (floors.at(0).x + (floors.at(0).h / 2.0f));
				start_y = start_floor.y + start_floor.h;
				break;
		}

		// spawn all floors
		std::vector<LevelFloorInternal> generated;
		for (const auto &f : floors)
		{
			LevelFloorInternal floor(f.texture, f.x, f.y, f.w, f.h, f.skip_prop_generation);
			reorient(floor.x, floor.y, floor.w, floor.h, side);
			floor.x += start_x;
			floor.y += start_y;

			generated.push_back(floor);
			if (floor.collide(existing))
				return {};
		}

		// place prop spawns
		for (const auto &prop : props)
		{
			const auto &original_floor = floors.at(prop.floor_index);
			auto p = prop.propdef.spawn(prop.side, original_floor.x + prop.x, original_floor.y + prop.y);
			reorient(p.x, p.y, p.w, p.h, side);
			p.x += start_x;
			p.y += start_y;
			p.side = reorient(side, p.side);

			auto &floor = generated.at(prop.floor_index);
			floor.prop_spawns.push_back(p);
		}

		return generated;
	}

	static LevelSide reorient(const LevelSide structure_side, const LevelSide prop_side)
	{
		int bias;
		switch (structure_side)
		{
			case LevelSide::left:
				bias = 2;
				break;
			case LevelSide::right:
				bias = 0;
				break;
			case LevelSide::bottom:
				bias = 3;
				break;
			case LevelSide::top:
				bias = 1;
				break;
		}

		int current_prop_side;
		switch (prop_side)
		{
			case LevelSide::left:
				current_prop_side = 2;
				break;
			case LevelSide::right:
				current_prop_side = 0;
				break;
			case LevelSide::bottom:
				current_prop_side = 3;
				break;
			case LevelSide::top:
				current_prop_side = 1;
				break;
		}

		const int new_prop_side = (current_prop_side + bias) % 4;

		const LevelSide sides[] { LevelSide::right, LevelSide::top, LevelSide::left, LevelSide::bottom };
		return sides[new_prop_side];
	}

	static void reorient(float &x, float &y, float &w, float &h, LevelSide side)
	{
		float temp;

		switch (side)
		{
			case LevelSide::left:
				x = (-x) - w;
				y = (-y)-h;
				break;
			case LevelSide::right:
				break;
			case LevelSide::bottom:
				temp = x;
				x = y;
				y = (-temp) - w;

				temp = w;
				w = h;
				h = temp;
				break;
			case LevelSide::top:
				temp = x;
				x = (-y) - h;
				y = temp;

				temp = w;
				w = h;
				h = temp;
				break;
			default:
				win::bug("no orientation");
		}
	}
};

struct StructureDefinitions
{
	static const StructureDefinitions &get() { static StructureDefinitions sd; return sd; }

	/*
	StructureDefinition testpattern
	{
		std::vector<LevelFloorInternal>
		{
			LevelFloorInternal(Texture::floor1, 0.0f, 0.0f, 2.5f, 5.0f, true),
			LevelFloorInternal(Texture::floor1, 2.5f, 2.0f, 2.5f, 5.0f, true),
			LevelFloorInternal(Texture::floor1, 5.0f, 4.0f, 2.5f, 5.0f, true),
		},

		std::vector<StructureFloorConnection>
		{
			StructureFloorConnection(0, 1),
			StructureFloorConnection(1, 2)
		},

		std::vector<StructureProp>
		{
			StructureProp(0, LevelSide::right, 2.1f, 4.3f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(0, LevelSide::top, 0.0f, 4.6f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(0, LevelSide::left, 0.0f, 0.0f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(0, LevelSide::bottom, 1.8f, 0.0f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(1, LevelSide::right, 2.1f, 4.3f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(1, LevelSide::top, 0.0f, 4.6f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(1, LevelSide::left, 0.0f, 0.0f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(1, LevelSide::bottom, 1.8f, 0.0f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(2, LevelSide::right, 2.1f, 4.3f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(2, LevelSide::top, 0.0f, 4.6f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(2, LevelSide::left, 0.0f, 0.0f, PropDefinitions::get().side_tables.at(0)),
			StructureProp(2, LevelSide::bottom, 1.8f, 0.0f, PropDefinitions::get().side_tables.at(0)),
		}
	};
	 */

	StructureDefinition small1
	{
		std::vector<LevelFloorInternal>
		{
			LevelFloorInternal(Texture::floor1, 0.0f, 0.0f, 4.0f, 2.0f, true),
			LevelFloorInternal(Texture::floor1, -1.0f, 2.0f, 6.0f, 2.0f, true),
		},

		std::vector<StructureFloorConnection>
		{
			StructureFloorConnection(0, 1)
		},

		std::vector<StructureProp>
		{
			StructureProp(1, LevelSide::top, 0.0f, 0.0f, PropDefinitions::get().side_tables.at(0))
		}
	};

	StructureDefinition small2
	{
		std::vector<LevelFloorInternal>
		{
			LevelFloorInternal(Texture::floor1, 0.0f, 0.0f, 3.6f, 6.0f, false),
			LevelFloorInternal(Texture::floor1, 3.6f, 1.5f, 4.0f, 3.0f, false),
			LevelFloorInternal(Texture::floor1, -2.0f, 6.0f, 7.0f, 3.0f, false)
		},

		std::vector<StructureFloorConnection>
		{
			StructureFloorConnection(0, 1),
			StructureFloorConnection(0, 2)
		}
	};

	StructureDefinition small3
	{
		std::vector<LevelFloorInternal>
		{
			LevelFloorInternal(Texture::floor1, 0.0f, 0.0f, 8.0f, 2.2f, false),
			LevelFloorInternal(Texture::floor1, 4.9f, 2.2f, 4.0f, 7.4f, false),
			LevelFloorInternal(Texture::floor1, -3.0f, -0.5f, 3.0f, 6.2f, false),
			LevelFloorInternal(Texture::floor1, -2.5f, 5.7f, 7.4f, 2.2f, false)
		},

		std::vector<StructureFloorConnection>
		{
			StructureFloorConnection(0, 1),
			StructureFloorConnection(0, 2),
			StructureFloorConnection(2, 3),
			StructureFloorConnection(1, 3),
		}
	};

	std::vector<StructureDefinition*> all
	{
		//&testpattern,
		&small1,
		&small2,
		&small3
	};

private:
	StructureDefinitions() = default;
};
