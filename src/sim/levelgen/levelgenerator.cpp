#include <stack>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <array>
#include <functional>

#include "../../render/texture.hpp"
#include "../../randomnumbergenerator.hpp"

#include "propdefs.hpp"
#include "structuredefs.hpp"
#include "levelobjects.hpp"
#include "levelgenerator.hpp"

///////////////////////////////////////////
/// GLOSSARY
///
/// Floor: A room "box" that the player can walk around in.
/// Prop: Furniture, rugs, random clutter, anything really.
/// Wall: Colliders between rooms
/// Connection: door-opening between rooms
/// Segment: a stretch of rooms (arranged in a particular pattern) that connect together.
/// Excluder: bounding-box exclusion zone around something (notably doors and props) that prevent other things from spawning too close.
///////////////////////////////////////////

///////////////////////////////////////////
/// random helper functions
///////////////////////////////////////////

static bool float_equals(float a, float b, float tolerance = 0.005f)
{
	return std::fabs(a - b) < tolerance;
}

static std::vector<LevelFloorConnector> get_connectors(const std::vector<LevelFloorConnector> &connectors, LevelSide side)
{
	std::vector<LevelFloorConnector> found;

	for (const auto &connector : connectors)
		if (connector.side == side)
			found.push_back(connector);

	std::sort(found.begin(), found.end(), [](const LevelFloorConnector &a, const LevelFloorConnector &b)
	{
		return a.start < b.start;
	});

	return found;
}

static std::array<LevelSide, 4> enumerate_connector_sides()
{
	std::array<LevelSide, 4> sides;

	sides.at(0) = LevelSide::left;
	sides.at(1) = LevelSide::right;
	sides.at(2) = LevelSide::bottom;
	sides.at(3) = LevelSide::top;

	return sides;
}

static LevelSide random_side(RandomNumberGenerator &rand)
{
	const int i = rand.uniform_int(0, 3);

	switch (i)
	{
		case 0:
			return LevelSide::left;
		case 1:
			return LevelSide::right;
		case 2:
			return LevelSide::bottom;
		case 3:
			return LevelSide::top;
		default:
			win::bug("no side");
	}

}

static LevelSide flip(LevelSide side)
{
	switch (side)
	{
	case LevelSide::left:
		return LevelSide::right;
	case LevelSide::right:
		return LevelSide::left;
	case LevelSide::bottom:
		return LevelSide::top;
	case LevelSide::top:
		return LevelSide::bottom;
	}

	win::bug("no side");
}

///////////////////////////////////////////
/// Prop generation
///////////////////////////////////////////

// generate those lil dooly-bobs between floors
static std::vector<LevelProp> generate_transition_strips(const LevelFloor &floor)
{
	std::vector<LevelProp> props;

	const LevelPropDefinition &def = PropDefinitions::floor_transition_strip;

	for (const LevelFloorConnector &con : floor.connectors)
	{
		LevelPropOrientation o;

		float x = 0.0f, y = 0.0f;
		switch (con.side)
		{
			case LevelSide::left:
				o = LevelPropOrientation::left;
				x = floor.x - (def.get_width(o) / 2.0f);
				y = con.start;
				break;
			case LevelSide::right:
				continue;
			case LevelSide::bottom:
				o = LevelPropOrientation::down;
				x = con.start;
				y = floor.y - (def.get_height(o) / 2.0f);
				break;
			case LevelSide::top:
				continue;
		}

		LevelProp strip(o, PropDefinitions::floor_transition_strip, x, y);

		props.push_back(strip);
	}

	return props;
}

// some floors have pre-defined prop spawns. handle those
static std::vector<LevelProp> generate_props_from_spawns(const LevelFloor &floor, const std::vector<LevelProp> &excluders)
{
	std::vector<LevelProp> props;

	for (const LevelProp &prop : floor.prop_spawns)
	{
		if (!prop.collide(props) && !prop.collide(excluders))
			props.push_back(prop);
	}

	return props;
}

// generate some brand-new props for a given floor. randomly.
static std::vector<LevelProp> generate_new_props(RandomNumberGenerator &rand, const LevelFloor &floor, const std::vector<LevelProp> &excluders)
{
	std::vector<LevelProp> props;

	// generate some rugs
	if (rand.one_in(2))
	{
		const LevelPropDefinition propdef = PropDefinitions::rugs.at(rand.uniform_int(0, PropDefinitions::rugs.size() - 1));

		LevelPropOrientation orientations[4] { LevelPropOrientation::left, LevelPropOrientation::right, LevelPropOrientation::down, LevelPropOrientation::up };
		const LevelPropOrientation orientation = orientations[rand.uniform_int(0, 3)];

		const float x = (floor.x + (floor.w / 2.0f)) - (propdef.get_width(orientation) / 2.0f);
		const float y = (floor.y + (floor.h / 2.0f)) - (propdef.get_height(orientation) / 2.0f);

		LevelProp rug(orientation, propdef, x, y);

		const float width_pct = propdef.get_width(orientation) / floor.w;
		const float height_pct = propdef.get_height(orientation) / floor.h;

		if (!rug.collide(props))
		{
			if (width_pct < 0.8f && height_pct < 0.8f)
				props.push_back(rug);
		}
	}

	// generate some side tables
	const int side_tables = rand.uniform_int(0, 4);
	for (int i = 0; i < side_tables; ++i)
	{
		const int attempts = 3;

		for (int j = 0; j < attempts; ++j)
		{
			const LevelPropDefinition &propdef = PropDefinitions::side_tables.at(rand.uniform_int(0, PropDefinitions::side_tables.size() - 1));
			const LevelSide side = random_side(rand);
			const float table_margin = 0.075f;

			float x, y;
			LevelPropOrientation orientation;

			switch (side)
			{
				case LevelSide::left:
					orientation = LevelPropOrientation::right;
					x = floor.x + table_margin;
					y = rand.uniform_real(floor.y, (floor.y + floor.h) - propdef.get_height(orientation));
					break;
				case LevelSide::right:
					orientation = LevelPropOrientation::left;
					x = ((floor.x + floor.w) - propdef.get_width(orientation)) - table_margin;
					y = rand.uniform_real(floor.y, (floor.y + floor.h) - propdef.get_height(orientation));
					break;
				case LevelSide::bottom:
					orientation = LevelPropOrientation::up;
					x = rand.uniform_real(floor.x, (floor.x + floor.w) - propdef.get_width(orientation));
					y = floor.y + table_margin;
					break;
				case LevelSide::top:
					orientation = LevelPropOrientation::down;
					x = rand.uniform_real(floor.x, (floor.x + floor.w) - propdef.get_width(orientation));
					y = ((floor.y + floor.h) - propdef.get_height(orientation)) - table_margin;
					break;
			}

			LevelProp prop(orientation, propdef, x, y);

			if (!prop.collide(props) && !prop.collide(excluders))
			{
				props.push_back(prop);
				break;
			}
		}
	}

	// generate some center tables
	if (rand.one_in(3))
	{
		const LevelPropDefinition &propdef = PropDefinitions::center_tables.at(rand.uniform_int(0, PropDefinitions::center_tables.size() - 1));

		LevelPropOrientation orientations[4] { LevelPropOrientation::left, LevelPropOrientation::right, LevelPropOrientation::down, LevelPropOrientation::up };
		const LevelPropOrientation orientation = orientations[rand.uniform_int(0, 3)];

		const float x = (floor.x + (floor.w / 2.0f)) - (propdef.get_width(orientation) / 2.0f);
		const float y = (floor.y + (floor.h / 2.0f)) - (propdef.get_height(orientation) / 2.0f);

		LevelProp table(orientation, propdef, x, y);

		if (!table.collide(props) && !table.collide(excluders))
			props.push_back(table);
	}

	return props;
}

// generate excluders around door openings
static std::vector<LevelProp> generate_door_excluders(const LevelFloor &floor)
{
	std::vector<LevelProp> excluders;

	for (const LevelFloorConnector &connector : floor.connectors)
	{
		float x, y, w, h;

		const float front_clearance = 0.3f; // distance in front of the door
		const float side_clearance = 0.1f; // extra padding around the sides of the door
		const float double_side_clearance = side_clearance * 2.0f;

		switch (connector.side)
		{
			case LevelSide::left:
				x = floor.x;
				y = connector.start - side_clearance;
				w = front_clearance;
				h = (connector.stop - connector.start) + double_side_clearance;
				break;
			case LevelSide::right:
				x = (floor.x + floor.w) - front_clearance;
				y = connector.start - side_clearance;
				w = front_clearance;
				h = (connector.stop - connector.start) + double_side_clearance;
				break;
			case LevelSide::bottom:
				x = connector.start - side_clearance;
				y = floor.y;
				w = (connector.stop - connector.start) + double_side_clearance;
				h = front_clearance;
				break;
			case LevelSide::top:
				x = connector.start - side_clearance;
				y = (floor.y + floor.h) - front_clearance;
				w = (connector.stop - connector.start) + double_side_clearance;
				h = front_clearance;
				break;
		}

		excluders.emplace_back(LevelPropOrientation::left, LevelPropDefinition(0, LevelPropDefinition::collision_class_excluder, LevelPropDefinition::collision_class_furniture, w, h, 0.1f, 0.1f), x, y);
	}

	return excluders;
}

// look at all floors and generate props for them (if appropriate)
static std::vector<LevelProp> generate_props(RandomNumberGenerator &rand, const std::vector<LevelFloor> &floors)
{
	std::vector<LevelProp> props;

	for (const LevelFloor &floor : floors)
	{
		const std::vector<LevelProp> door_excluders = generate_door_excluders(floor);

		// debugging
		//for (const LevelProp &excluder : door_excluders)
		//props.push_back(excluder);

		std::vector<LevelProp> spawned_props;
		if (floor.prop_spawns.size() > 0)
		{
			spawned_props = generate_props_from_spawns(floor, door_excluders);

			for (const LevelProp &prop : spawned_props)
				props.push_back(prop);
		}

		if (!floor.skip_prop_generation)
		{
			std::vector<LevelProp> all_excluders;
			for (const LevelProp &prop : door_excluders) all_excluders.push_back(prop);
			for (const LevelProp &prop : spawned_props) all_excluders.push_back(prop);

			const std::vector<LevelProp> floorprops = generate_new_props(rand, floor, all_excluders);

			for (const LevelProp &prop : floorprops)
				props.push_back(prop);
		}

		// generate them lil transition strips between the floor textures
		const std::vector<LevelProp> strips = generate_transition_strips(floor);
		for (const LevelProp &strip : strips)
			props.push_back(strip);
	}

	return props;
}

///////////////////////////////////////////
/// Wall generation
///////////////////////////////////////////

// generate walls between the floors
static std::vector<LevelWall> generate_walls(const std::vector<LevelFloor> &floors)
{
	std::vector<LevelWall> walls;

	for (const auto &floor : floors)
	{
		for (const auto side : enumerate_connector_sides())
		{
			auto connectors	= get_connectors(floor.connectors, side);

			if (side == LevelSide::bottom || side == LevelSide::top)
			{
				const float y = side == LevelSide::bottom ? floor.y - LevelWall::HALFWIDTH : ((floor.y + floor.h) - LevelWall::HALFWIDTH);
				float startx = floor.x;

				walls.emplace_back(startx, y, connectors.size() > 0 ? (connectors.at(0).start - startx) : floor.w, LevelWall::WIDTH);
				if (connectors.size() > 0)
					startx = connectors.at(0).stop;

				for (int i = 0; i < connectors.size(); ++i)
				{
					const auto &connector = connectors[i];
					walls.emplace_back(startx, y, i + 1 < connectors.size() ? connectors.at(i + 1).start - startx : ((floor.x + floor.w) - startx), LevelWall::WIDTH);
					if (i + 1 < connectors.size())
						startx = connectors.at(i + 1).stop;
				}
			}
			else
			{
				const float x = side == LevelSide::left ? floor.x - LevelWall::HALFWIDTH : ((floor.x + floor.w) - LevelWall::HALFWIDTH);
				float starty = floor.y;

				walls.emplace_back(x, starty, LevelWall::WIDTH, connectors.size() > 0 ? (connectors.at(0).start - starty) : floor.h);
				if (connectors.size() > 0)
					starty = connectors.at(0).stop;

				for (int i = 0; i < connectors.size(); ++i)
				{
					const auto &connector = connectors[i];
					walls.emplace_back(x, starty, LevelWall::WIDTH, i + 1 < connectors.size() ? connectors.at(i + 1).start - starty : ((floor.y + floor.h) - starty));
					if (i + 1 < connectors.size())
						starty = connectors.at(i + 1).stop;
				}
			}
		}
	}

	return walls;
}

///////////////////////////////////////////
/// Core floor layout logic
///////////////////////////////////////////

// get rid of floors that have no connection to another
static std::vector<LevelFloor> prune(const std::vector<LevelFloor> &floors)
{
	std::vector<LevelFloor> pruned;

	for (const auto &floor : floors)
	{
		if (!floor.connectors.empty())
			pruned.push_back(floor);
	}

	return pruned;
}

// can two floors connect properly?
static bool can_connect(const LevelFloor &floor1, const LevelFloor &floor2, LevelFloorConnector &c1, LevelFloorConnector &c2)
{
	// determine which side floor 2 is on, relative to floor 1
	LevelSide side;
	if (float_equals(floor1.x + floor1.w, floor2.x)) // right?
		side = LevelSide::right;
	else if (float_equals(floor1.x, floor2.x + floor2.w)) // left?
		side = LevelSide::left;
	else if (float_equals(floor1.y, floor2.y + floor2.h)) // bottom?
		side = LevelSide::bottom;
	else if (float_equals(floor1.y + floor1.h, floor2.y)) // top?
		side = LevelSide::top;
	else
		return false;

	const float MIN_WALL_LENGTH = 1.8f; // minimum wall length needed to support a door (with some padding)
	const float DOOR_LENGTH = 1.0f;

	float start, stop;
	if (side == LevelSide::bottom || side == LevelSide::top)
	{
		const float farleft = std::max(floor1.x, floor2.x);
		const float farright = std::min(floor1.x + floor1.w, floor2.x + floor2.w);
		if (farright - farleft < MIN_WALL_LENGTH)
			return false;

		start = ((farright + farleft) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}
	else if (side == LevelSide::left || side == LevelSide::right)
	{
		const float lower = std::max(floor1.y, floor2.y);
		const float upper = std::min(floor1.y + floor1.h, floor2.y + floor2.h);

		if (upper - lower < MIN_WALL_LENGTH)
			return false;

		start = ((upper + lower) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}

	LevelSide flipside;
	switch (side)
	{
		case LevelSide::top:
			flipside = LevelSide::bottom;
			break;
		case LevelSide::bottom:
			flipside = LevelSide::top;
			break;
		case LevelSide::left:
			flipside = LevelSide::right;
			break;
		case LevelSide::right:
			flipside = LevelSide::left;
			break;
	}

	LevelFloorConnector floor1_connector(side, start, stop);
	LevelFloorConnector floor2_connector(flipside, start, stop);

	if (floor1_connector.collide(floor1.connectors, 0.5f))
		return false;
	if (floor2_connector.collide(floor2.connectors, 0.5f))
		return false;

	c1 = floor1_connector;
	c2 = floor2_connector;

	return true;
}

// mawwaige
static bool connect(LevelFloor &floor1, LevelFloor &floor2)
{
	LevelFloorConnector floor1_connector;
	LevelFloorConnector floor2_connector;

	if (can_connect(floor1, floor2, floor1_connector, floor2_connector))
	{
		floor1.connectors.push_back(floor1_connector);
		floor2.connectors.push_back(floor2_connector);
		return true;
	}

	return false;
}

// given an existing floor, find all those (among all other existing floors) that neighbor it in a manner appropriate to join them together in holy matrimony
static std::vector<LevelFloor*> find_neighbors(std::vector<LevelFloor> &floors, const LevelFloor& floor, LevelSide side)
{
	std::vector<LevelFloor*> matches;

	for (auto &f : floors)
	{
		if (&floor == &f)
			continue;

		LevelFloorConnector c1;
		LevelFloorConnector c2;

		if (can_connect(floor, f, c1, c2))
		{
			if (c1.side == side)
				matches.push_back(&f);
		}
	}

	return matches;
}

// of some existing floors, find one to serve as a starting point for a future segment
static int find_start_candidate(RandomNumberGenerator &rand, const std::vector<LevelFloor> &floors, const int attempt_num)
{
	if (attempt_num < 0)
		win::bug("invalid param");

	if (floors.empty())
		win::bug("no possible start candidate");

	if (floors.size() == 1)
		return 0;

	const int low = std::max(1, ((int)floors.size()) - 8);
	const int high = floors.size() - 1;

	std::vector<int> one_conn;
	std::vector<int> two_conn;
	std::vector<int> three_conn;

	for (int i = low; i <= high; ++i)
	{
		if (floors.at(i).connectors.size() == 1)
			one_conn.push_back(i);
		else if (floors.at(i).connectors.size() == 2)
			two_conn.push_back(i);
		else if (floors.at(i).connectors.size() == 3)
			three_conn.push_back(i);
	}

	std::shuffle(one_conn.begin(), one_conn.end(), rand.mersenne);
	std::shuffle(two_conn.begin(), two_conn.end(), rand.mersenne);
	std::shuffle(three_conn.begin(), three_conn.end(), rand.mersenne);

	std::vector<int> candidates;

	for (int c : one_conn)
		candidates.push_back(c);
	for (int c : two_conn)
		candidates.push_back(c);
	for (int c : three_conn)
		candidates.push_back(c);

	if (attempt_num >= candidates.size())
		return -1;

	return candidates.at(attempt_num);
}

/*
static bool test_floor(const std::vector<LevelFloor> &floors, const LevelFloor &floor)
{
	for (const auto &f : floors)
	{
		if (f.collide(floor))
			return false;
	}

	return true;
}
 */

// make some predefined hand crafted rooms
static std::vector<LevelFloor> generate_structure(RandomNumberGenerator &rand, const std::vector<LevelFloor> &existing_floors, const LevelFloor &start_floor, const LevelSide start_side, int force_index = -1)
{
	std::vector<LevelFloor> generated;

	const Structure &s = structure_defs.at(force_index == -1 ? rand.uniform_int(0, structure_defs.size() - 1) : force_index);

	if (s.floors.size() < 1)
		win::bug("short structure");

	const StructureFloor &first_structure_floor = s.floors.at(0);

	float origin_x, origin_y;
	switch (start_side)
	{
		case LevelSide::top:
			origin_x = (start_floor.x + (start_floor.w / 2.0f)) - (first_structure_floor.width / 2.0f);
			origin_y = start_floor.y + start_floor.h;
			break;
		case LevelSide::left:
			origin_x = start_floor.x;
			origin_y = (start_floor.y + (start_floor.h / 2.0f)) - (first_structure_floor.width / 2.0f);
			break;
		case LevelSide::bottom:
			origin_x = (start_floor.x + (start_floor.w / 2.0f)) + (first_structure_floor.width / 2.0f);
			origin_y = start_floor.y;
			break;
		case LevelSide::right:
			origin_x = start_floor.x + start_floor.w;
			origin_y = (start_floor.y + (start_floor.h / 2.0f)) + (first_structure_floor.width / 2.0f);
			break;
	}

	for (const StructureFloor &sfloor : s.floors)
	{
		const LevelFloor floor = sfloor.get_floor(start_side, origin_x, origin_y);

		if (floor.collide(existing_floors))
			return {};
		if (floor.collide(generated) && false)
			win::bug("mangled structure");

		generated.push_back(floor);
	}

	for (const StructureFloorConnection &c : s.connections)
	{
		connect(generated.at(c.index_x), generated.at(c.index_y));
	}

	return generated;
}

// make a winding line of rooms
static std::vector<LevelFloor> generate_linear(RandomNumberGenerator &rand, const std::vector<LevelFloor> &existing_floors, const LevelFloor &start_floor, const LevelSide start_side)
{
	std::vector<LevelFloor> generated;

	const int count = rand.uniform_int(0, 3) == 0 ? rand.uniform_int(2, 4) : rand.uniform_int(5, 12);

	int from_index = -1;
	for (int num = 0; num < count; ++num)
	{
		LevelFloor *const head = from_index == -1 ? NULL : &generated.at(from_index);
		bool success = false;
		for (int attempt = 0; attempt < 6; ++attempt)
		{
			const LevelSide side = head == NULL ? start_side : random_side(rand);

			const float from_x = head ? head->x : start_floor.x;
			const float from_y = head ? head->y : start_floor.y;
			const float from_w = head ? head->w : start_floor.w;
			const float from_h = head ? head->h : start_floor.h;

			const float width = rand.uniform_int(0, 2) == 0 ? rand.uniform_real(3.0f, 4.0f) : rand.uniform_real(2.0f, 3.0f);
			const float height = rand.uniform_int(0, 2) == 0 ? rand.uniform_real(3.0f, 4.0f) : rand.uniform_real(2.0f, 3.0f);
			float x, y;

			const float MIN_WALL_LENGTH = 1.8f; // minimum wall length needed to support a door (with some padding)

			const float lowx = (from_x + MIN_WALL_LENGTH) - width;
			const float highx = ((from_x + from_w) - MIN_WALL_LENGTH);

			const float lowy = (from_y + MIN_WALL_LENGTH) - height;
			const float highy = (from_y + from_h) - MIN_WALL_LENGTH;

			switch (side)
			{
				case LevelSide::left:
					x = from_x - width;
					y = rand.uniform_real(lowy, highy);
					break;
				case LevelSide::right:
					x = from_x + from_w;
					y = rand.uniform_real(lowy, highy);
					break;
				case LevelSide::bottom:
					x = rand.uniform_real(lowx, highx);
					y = from_y - height;
					break;
				case LevelSide::top:
					x = rand.uniform_real(lowx, highx);
					y = from_y + from_h;
					break;
			}

			LevelFloor floor(rand.uniform_int(0, 1), x, y, width, height);

			if (floor.collide(existing_floors))
				continue;
			if (floor.collide(generated))
				continue;

			if (head != NULL)
				if (!connect(floor, *head))
					win::bug("no connection");

			generated.push_back(floor);
			from_index = generated.size() - 1;

			success = true;
			break;
		}

		if (!success)
		{
			if (from_index == -1)
				return generated;

			--from_index;

			if (from_index == -1)
				return generated;
		}
	}

	return generated;
}

// make some grid-style rooms
static std::vector<LevelFloor> generate_grid(RandomNumberGenerator &rand, const std::vector<LevelFloor> &existing_floors, const LevelFloor &start_floor, LevelSide start_side)
{
	std::vector<LevelFloor> generated;

	const int horizontal_tiles = rand.uniform_int(3, 6);
	const int vertical_tiles = rand.uniform_int(3, 6);
	const float tile_width = rand.uniform_real(2.6f, 3.5f);
	const float tile_height = rand.uniform_real(2.6f, 3.5f);

	float start_x, start_y;
	if (start_side == LevelSide::top)
	{
		start_x = (start_floor.x + (start_floor.w / 2.0f)) - ((horizontal_tiles * tile_width) / 2.0f);
		start_y = start_floor.y + start_floor.h;

		if (horizontal_tiles % 2 == 0)
			start_x -= (tile_width / 2.0f);
	}
	else if (start_side == LevelSide::left)
	{
		start_x = start_floor.x - (horizontal_tiles * tile_width);
		start_y = (start_floor.y + (start_floor.h / 2.0f)) - ((vertical_tiles * tile_height) / 2.0f);

		if (vertical_tiles % 2 == 0)
			start_y -= (tile_height / 2.0f);
	}
	else if (start_side == LevelSide::right)
	{
		start_x = start_floor.x + start_floor.w;
		start_y = (start_floor.y + (start_floor.h / 2.0f)) - ((vertical_tiles * tile_height) / 2.0f);

		if (vertical_tiles % 2 == 0)
			start_y -= (tile_height / 2.0f);
	}
	else if (start_side == LevelSide::bottom)
	{
		start_x = (start_floor.x + (start_floor.w / 2.0f)) - ((horizontal_tiles * tile_width) / 2.0f);
		start_y = start_floor.y - (vertical_tiles * tile_height);

		if (horizontal_tiles % 2 == 0)
			start_x -= (tile_width / 2.0f);
	}
	else win::bug("no side");

	const float initial_start_y = start_y;

	for (int i = 0; i < horizontal_tiles; ++i)
	{
		for (int j = 0; j < vertical_tiles; ++j)
		{
			const bool long_horizontal = rand.uniform_int(0, 6) == 0;
			const bool long_vertical = rand.uniform_int(0, 6) == 0;

			LevelFloor floor(rand.uniform_int(0, 1), start_x, start_y, long_horizontal ? tile_width * 2.0f : tile_width, long_vertical ? tile_height * 2.0f : tile_height);

			bool collision = false;
			if (floor.collide(existing_floors))
				collision = true;

			if (floor.collide(generated))
				collision = true;

			if (!collision)
				generated.push_back(floor);

			start_y += tile_height;
		}

		start_y = initial_start_y;
		start_x += tile_width;
	}

	auto neighbors = find_neighbors(generated, start_floor, start_side);
	if (neighbors.empty())
		return generated;
	auto neighbor = neighbors.at(rand.uniform_int(0, neighbors.size() - 1));
	//if (!connect(start_floor, *neighbor)) return generated;
	LevelSide camefrom = flip(start_side);

	std::stack<LevelFloor*> heads;
	heads.push(neighbor);

	for (int i = 0; i < generated.size(); ++i)
	{
		auto head = heads.top();
		bool success = false;

		for (int attempt = 0; attempt < 2; ++attempt)
		{
			LevelSide dir = camefrom;
			while (dir == camefrom)
				dir = random_side(rand);

			neighbors = find_neighbors(generated, *head, dir);
			if (neighbors.empty())
				continue;
			neighbor = neighbors.at(rand.uniform_int(0, neighbors.size() - 1));

			if (connect(*head, *neighbor))
			{
				heads.push(neighbor);
				camefrom = flip(dir);
				success = true;
				break;
			}
		}

		if (!success)
		{
			heads.pop();
			if (heads.empty())
				break;
		}
	}

	return generated;
}

// tries to find an existing floor to connect to a future new segment
// generates the new segment, and tries to connect it to the "start candidate" (above)
typedef std::function<std::vector<LevelFloor>(const LevelFloor&, LevelSide)> GeneratorFunction;
static bool generate_segment(RandomNumberGenerator &rand, std::vector<LevelFloor> &existing_floors, const int minimum_added, const LevelSide side, const GeneratorFunction &generator)
{
	int attempts = 0;
	std::vector<LevelFloor> generated;
	int start_candidate_index;

	do
	{
		start_candidate_index = find_start_candidate(rand, existing_floors, attempts++);

		if (start_candidate_index == -1)
			return false;

		generated = generator(existing_floors.at(start_candidate_index), side);
	} while (generated.size() < minimum_added);

	if (!connect(existing_floors.at(start_candidate_index), *find_neighbors(generated, existing_floors.at(start_candidate_index), side).at(0)))
		win::bug("Failed to connect segments");

	for (const auto &f : generated)
		existing_floors.push_back(f);

	return true;
}

// is called many times until finally a satisfactory result is achieved
// generates segments and mushes em together
static std::vector<LevelFloor> generate_impl(RandomNumberGenerator &rand)
{
	std::vector<LevelFloor> floors;

	// starting room
	floors.emplace_back(0, -1.0f, -1.0f, 2.0f, 2.0f);

	/*
	// ================ structure testing
	for (const auto &f : generate_structure(floors.at(0), LevelSide::top, 0))
		floors.push_back(f);
	connect(floors.at(0), floors.at(1));
	return true;
	*/

	auto grid_generator = [&rand, &floors](const LevelFloor& start_floor, LevelSide side)
	{
		return prune(generate_grid(rand, floors, start_floor, side));
	};

	auto linear_generator = [&rand, &floors](const LevelFloor &start_floor, LevelSide side)
	{
		return generate_linear(rand, floors, start_floor, side);
	};

	auto structure_generator = [&rand, &floors](const LevelFloor &start_floor, LevelSide side)
	{
		return generate_structure(rand, floors, start_floor, side);
	};

	const std::vector<std::pair<GeneratorFunction, int>> generators =
	{
		std::make_pair(grid_generator, 10),
		std::make_pair(structure_generator, 1),
		std::make_pair(linear_generator, 5)
	};

	bool first = true;
	int last_index = -1;
	while (floors.size() < 50)
	{
		// pick a generator
		int generator_index;
		do
		{
			generator_index = rand.uniform_int(0, generators.size() - 1);
		} while (generator_index == last_index);
		last_index = generator_index;

		if (!generate_segment(rand, floors, generators.at(generator_index).second, first ? LevelSide::top : random_side(rand), generators.at(generator_index).first))
		{
			// nevermind clear it and bug out
			floors.clear();
			return floors;
		}

		first = false;
	}

	return floors;
}

// append the seed to a hidden file
static void log_seed(int seed)
{
#if defined WINPLAT_WINDOWS
	const char *local_app_data = std::getenv("LOCALAPPDATA");
	std::filesystem::path basepath = userprofile ? std::filesystem::path(userprofile) / ".darktimes-seeds.txt" : "";
#elif defined WINPLAT_LINUX
	const char *home = std::getenv("HOME");
	const std::filesystem::path basepath = home ? std::filesystem::path(home) / ".darktimes-seeds.txt" : "";
#endif
	char timestring[100];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(timestring, sizeof(timestring), "%Y-%m-%d %I:%M:%S %p", t);

	std::ofstream out(basepath, std::ofstream::app);
	if (out)
		out << timestring << "\t" << seed << std::endl;
}

// entry point
void level_generate(
	int seed,
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &atlas_renderables,
	win::Pool<RenderableComponent> &tile_renderables
)
{
	RandomNumberGenerator rand(seed);

	std::vector<LevelFloor> floors;
	do
	{
		floors = generate_impl(rand);
	} while (floors.empty());

	const std::vector<LevelWall> walls = generate_walls(floors);

	const std::vector<LevelProp> props = generate_props(rand, floors);

	// map to entities and components

	for (const auto &wall : walls)
	{
		auto &entity = entities.add();
		entity.add(physicals.add(entity, wall.x, wall.y, wall.w, wall.h, 0.0f));
		entity.add(atlas_renderables.add(entity, Texture::level_prop_sidetable));
	}

	for (const auto &prop : props)
	{
		auto &entity = entities.add();
		entity.add(physicals.add(entity, prop.x, prop.y, prop.width, prop.height, 0.0f));
		entity.add(atlas_renderables.add(entity, Texture::level_prop_sidetable));
	}

	for (const auto &floor : floors)
	{
		auto &entity = entities.add();
		entity.add(physicals.add(entity, floor.x, floor.y, floor.w, floor.h, 0.0f));
		entity.add(tile_renderables.add(entity, floor.texture));
	}

	log_seed(seed);
}
