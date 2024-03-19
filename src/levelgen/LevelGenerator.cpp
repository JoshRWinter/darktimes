#include <stack>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <array>
#include <functional>

#include "../Texture.hpp"

#include "LevelGenerator.hpp"
#include "PropDefs.hpp"
#include "StructureDefs.hpp"
#include "LevelObjectsInternal.hpp"

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
/// COOL NUMBERS
///////////////////////////////////////////
static constexpr float MIN_WALL_LENGTH = 1.8f;
static constexpr float DOOR_LENGTH = 1.0f;

// entry point
void LevelGenerator::generate(int seed)
{
	rand.reseed(seed);
	log_seed(seed);

	const auto generated_floors = generate_impl();

	for (const auto &f : generated_floors)
		floors.push_back(f);

	for (const auto &w : generate_walls(generated_floors))
		walls.push_back(w);

	for (const auto &p : generate_props(generated_floors))
		props.push_back(p);

#ifndef NDEBUG
	fputs("=============================\n", stderr);
	fprintf(stderr, "LevelGenerator health report:\nrestarts: %d\niterations: %d\n", health.restarts, health.iterations);
	fputs("=============================\n", stderr);
#endif
}

///////////////////////////////////////////
/// Core floor layout logic
///////////////////////////////////////////

// generates segments and mushes em together
std::vector<LevelFloorInternal> LevelGenerator::generate_impl()
{
	// reset the level state
	floors.clear();
	walls.clear();
	props.clear();

	// reset the health object
	health = decltype(health)();

	std::vector<LevelFloorInternal> floors;

	// starting room
	LevelFloorInternal spawn(Texture::floor2, -1.0f, -1.0f, 2.0f, 2.0f);
	floors.push_back(spawn);

	/*
	// ================ structure testing
	for (const auto &f : generate_structure(floors, floors.at(0), LevelSide::top, 0))
		floors.push_back(f);
	if (!connect(floors.at(0), floors.at(1)) && false) win::bug("no connection");
	return floors;
	*/

	auto grid_generator = [this, &floors](const LevelFloorInternal& start_floor, LevelSide side)
	{
		return prune(generate_grid(floors, start_floor, side));
	};

	auto linear_generator = [this, &floors](const LevelFloorInternal &start_floor, LevelSide side)
	{
		return generate_linear(floors, start_floor, side);
	};

	auto structure_generator = [this, &floors](const LevelFloorInternal &start_floor, LevelSide side)
	{
		return generate_structure(floors, start_floor, side);
	};

	const std::array<std::pair<GeneratorFunction, int>, 3> generators =
	{
		std::make_pair(grid_generator, 5),
		std::make_pair(structure_generator, 1),
		std::make_pair(linear_generator, 2)
	};

	int last_generator = -1;
	int attempt = 0;
	while (floors.size() < 50)
	{
		if (++attempt == 20)
		{
			// this world is impossible, scorch the earth
			floors.clear();
			floors.push_back(spawn);

			last_generator = -1;
			attempt = 0;

			++health.restarts;
			health.iterations = 0;
		}

		// pick a generator
		int generator_index;
		do
		{
			generator_index = rand.uniform_int(0, generators.size() - 1);
		} while (generator_index == last_generator);
		last_generator = generator_index;

		const auto &generator = generators.at(generator_index);
		generate_segment(floors, generator.second, floors.size() == 1 ? LevelSide::top : random_side(), generator.first);

		++health.iterations;
	}

	return floors;
}

// may or may not add a new segment to the existing layout
void LevelGenerator::generate_segment(std::vector<LevelFloorInternal> &existing_floors, const int minimum_added, const LevelSide side, const GeneratorFunction &generator)
{
	// attempt to find a floor among the existing floors that we can start to "build" off of
	const std::vector<int> start_candidates = find_start_candidates(existing_floors);
	for (const auto start_candidate : start_candidates)
	{
		auto &start_floor = existing_floors.at(start_candidate);
		auto generated = generator(start_floor, side);

		if (generated.size() >= minimum_added)
		{
			const auto neighbors = find_neighbors(generated, start_floor);
			if (neighbors.empty())
				continue;

			if (!connect(start_floor, generated.at(neighbors.at(rand.uniform_int(0, neighbors.size() - 1)))))
				win::bug("Failed to connect segments");

			for (const auto &f : generated)
				existing_floors.push_back(f);

			return;
		}
	}
}

// make some grid-style rooms
std::vector<LevelFloorInternal> LevelGenerator::generate_grid(const std::vector<LevelFloorInternal> &existing_floors, const LevelFloorInternal &start_floor, LevelSide start_side)
{
	std::vector<LevelFloorInternal> generated;

	const int horizontal_tiles = rand.uniform_int(3, 6);
	const int vertical_tiles = rand.uniform_int(3, 6);
	const float floor_width = rand.uniform_real(MIN_WALL_LENGTH * 2.0f, 3.5f);
	const float floor_height = rand.uniform_real(MIN_WALL_LENGTH * 2.0f, 3.5f);

	// determine where the first floor should start
	float start_x, start_y;
	if (start_side == LevelSide::top)
	{
		// slide the grid to the left a bit for extra randomz
		const float offset = floor_width * rand.uniform_int(0, horizontal_tiles - 1);

		start_x = ((start_floor.x + (start_floor.w / 2.0f)) - (floor_width / 2.0f)) - offset;
		start_y = start_floor.y + start_floor.h;
	}
	else if (start_side == LevelSide::left)
	{
		// slide the grid down a bit for extra randomz
		const float offset = floor_height * rand.uniform_int(0, vertical_tiles - 1);

		start_x = start_floor.x - floor_width;
		start_y = ((start_floor.y + (start_floor.h / 2.0f)) - (floor_height / 2.0f)) - offset;
	}
	else if (start_side == LevelSide::right)
	{
		const float offset = floor_height * rand.uniform_int(0, vertical_tiles - 1);

		start_x = start_floor.x + start_floor.w;
		start_y = ((start_floor.y + (start_floor.h / 2.0f)) - (floor_height / 2.0f)) - offset;
	}
	else if (start_side == LevelSide::bottom)
	{
		const float offset = floor_width * rand.uniform_int(0, horizontal_tiles - 1);

		start_x = ((start_floor.x + (start_floor.w / 2.0f)) - (floor_width / 2.0f)) - offset;
		start_y = start_floor.y - floor_height;
	}
	else win::bug("no side");

	const float initial_start_y = start_y;
	for (int i = 0; i < horizontal_tiles; ++i)
	{
		for (int j = 0; j < vertical_tiles; ++j)
		{
			const bool first_floor = i == 0 && j == 0;

			const bool long_horizontal = rand.uniform_int(0, 6) == 0;
			const bool long_vertical = rand.uniform_int(0, 6) == 0;

			LevelFloorInternal floor(Texture::floor1, start_x, start_y, long_horizontal ? floor_width * 2.0f : floor_width, long_vertical ? floor_height * 2.0f : floor_height);

			bool collision = false;
			if (floor.collide(existing_floors))
			{
				if (first_floor)
					return {}; // give up

				collision = true;
			}

			if (floor.collide(generated))
			{
				if (first_floor)
					win::bug("can't collide on the first floor lmao");

				collision = true;
			}

			if (!collision)
				generated.push_back(floor);

			start_y += floor_height;
		}

		start_y = initial_start_y;
		start_x += floor_width;
	}

	// wind through the grid connecting stuff up
	// start at the floor that neighbors the start floor
	auto neighbors = find_neighbors(generated, start_floor);
	if (neighbors.empty())
		return {}; // sad

	auto floor = &generated.at(neighbors.at(0));
	while (true)
	{
		neighbors = find_neighbors(generated, *floor);

		if (neighbors.empty())
			break; // reached a dead end. give up

		auto &neighbor = generated.at(neighbors.at(rand.uniform_int(0, neighbors.size() - 1)));
		if (!connect(*floor, neighbor))
			win::bug("lol wut");

		floor = &neighbor;
	}

	return generated;
}

// make a winding line of rooms
std::vector<LevelFloorInternal> LevelGenerator::generate_linear(const std::vector<LevelFloorInternal> &existing_floors, const LevelFloorInternal &start_floor, const LevelSide start_side)
{
	std::vector<LevelFloorInternal> generated;

	const int count = rand.uniform_int(0, 3) == 0 ? rand.uniform_int(2, 4) : rand.uniform_int(5, 12);

	int from_index = -1;
	for (int num = 0; num < count; ++num)
	{
		LevelFloorInternal *const head = from_index == -1 ? NULL : &generated.at(from_index);
		bool success = false;
		for (int attempt = 0; attempt < 6; ++attempt)
		{
			const LevelSide side = head == NULL ? start_side : random_side();

			const float from_x = head ? head->x : start_floor.x;
			const float from_y = head ? head->y : start_floor.y;
			const float from_w = head ? head->w : start_floor.w;
			const float from_h = head ? head->h : start_floor.h;

			const float width = rand.uniform_int(0, 2) == 0 ? rand.uniform_real(3.0f, 4.0f) : rand.uniform_real(2.0f, 3.0f);
			const float height = rand.uniform_int(0, 2) == 0 ? rand.uniform_real(3.0f, 4.0f) : rand.uniform_real(2.0f, 3.0f);
			float x, y;

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

			LevelFloorInternal floor(Texture::floor1, x, y, width, height);

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
				return generated; // give up, maybe we've done enough

			--from_index;
		}
	}

	return generated;
}

// make some predefined hand crafted rooms
std::vector<LevelFloorInternal> LevelGenerator::generate_structure(const std::vector<LevelFloorInternal> &existing_floors, const LevelFloor &start_floor, const LevelSide side, int index)
{
	const auto &defs = StructureDefinitions::get();

	// pick a structure definition
	const StructureDefinition &s = *defs.all.at(index == -1 ? rand.uniform_int(0, defs.all.size() - 1) : index);

	// spawn the structure
	std::vector<LevelFloorInternal> generated = s.spawn(existing_floors, start_floor, side);

	// structure spawn failed for some reason (floor intersection)
	if (generated.empty())
		return {};

	// connect the floors
	for (const auto &c : s.connections)
	{
		if (!connect(generated.at(c.index_x), generated.at(c.index_y)))
			win::bug("Structure " + std::to_string(index) + ": couldn't connect floors " + std::to_string(c.index_x) + " and " + std::to_string(c.index_y));
	}

	return generated;
}

// of some existing floors, find one to serve as a starting point for a future segment
std::vector<int> LevelGenerator::find_start_candidates(const std::vector<LevelFloorInternal> &floors)
{
	if (floors.empty())
		win::bug("no possible start candidate");

	if (floors.size() == 1)
		return { 0 };

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

	return candidates;
}

// given an existing floor, find all those (among all other existing floors) that neighbor it in a manner appropriate to join them together in holy matrimony
std::vector<int> LevelGenerator::find_neighbors(const std::vector<LevelFloorInternal> &floors, const LevelFloorInternal& floor/*, LevelSide side*/)
{
	std::vector<int> matches;

	int index = -1;
	for (auto &f : floors)
	{
		++index;

		if (&floor == &f)
			continue;

		LevelFloorConnector c1;
		LevelFloorConnector c2;

		if (can_connect(floor, f, c1, c2))
		{
			//if (c1.side == side)
			matches.push_back(index);
		}
	}

	return matches;
}

// mawwaige
bool LevelGenerator::connect(LevelFloorInternal &floor1, LevelFloorInternal &floor2)
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

// can two floors connect properly?
bool LevelGenerator::can_connect(const LevelFloorInternal &floor1, const LevelFloorInternal &floor2, LevelFloorConnector &c1, LevelFloorConnector &c2)
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

// get rid of floors that have no connection to another
std::vector<LevelFloorInternal> LevelGenerator::prune(const std::vector<LevelFloorInternal> &floors)
{
	std::vector<LevelFloorInternal> pruned;

	for (const auto &floor : floors)
	{
		if (!floor.connectors.empty())
			pruned.push_back(floor);
	}

	return pruned;
}

///////////////////////////////////////////
/// Wall generation
///////////////////////////////////////////

// generate walls between the floors
std::vector<LevelWallInternal> LevelGenerator::generate_walls(const std::vector<LevelFloorInternal> &floors)
{
	std::vector<LevelWallInternal> walls;

	for (const auto &floor : floors)
	{
		for (const auto side : enumerate_connector_sides())
		{
			auto connectors	= get_connectors(floor.connectors, side);

			if (side == LevelSide::bottom || side == LevelSide::top)
			{
				const float y = side == LevelSide::bottom ? floor.y - LevelWallInternal::HALFWIDTH : ((floor.y + floor.h) - LevelWallInternal::HALFWIDTH);
				float startx = floor.x;

				walls.emplace_back(startx, y, !connectors.empty() ? (connectors.at(0).start - startx) : floor.w, LevelWallInternal::WIDTH);
				if (!connectors.empty())
					startx = connectors.at(0).stop;

				for (int i = 0; i < connectors.size(); ++i)
				{
					const auto &connector = connectors[i];
					walls.emplace_back(startx, y, i + 1 < connectors.size() ? connectors.at(i + 1).start - startx : ((floor.x + floor.w) - startx), LevelWallInternal::WIDTH);
					if (i + 1 < connectors.size())
						startx = connectors.at(i + 1).stop;
				}
			}
			else
			{
				const float x = side == LevelSide::left ? floor.x - LevelWallInternal::HALFWIDTH : ((floor.x + floor.w) - LevelWallInternal::HALFWIDTH);
				float starty = floor.y;

				walls.emplace_back(x, starty, LevelWallInternal::WIDTH, !connectors.empty() ? (connectors.at(0).start - starty) : floor.h);
				if (!connectors.empty())
					starty = connectors.at(0).stop;

				for (int i = 0; i < connectors.size(); ++i)
				{
					const auto &connector = connectors[i];
					walls.emplace_back(x, starty, LevelWallInternal::WIDTH, i + 1 < connectors.size() ? connectors.at(i + 1).start - starty : ((floor.y + floor.h) - starty));
					if (i + 1 < connectors.size())
						starty = connectors.at(i + 1).stop;
				}
			}
		}
	}

	return walls;
}

///////////////////////////////////////////
/// Prop generation
///////////////////////////////////////////

// look at all floors and generate props for them (if appropriate)
std::vector<LevelPropInternal> LevelGenerator::generate_props(const std::vector<LevelFloorInternal> &floors)
{
	std::vector<LevelPropInternal> props;

	for (const auto &floor : floors)
	{
		const std::vector<LevelPropExcluder> door_excluders = generate_door_excluders(floor);

		// debugging
		//for (const LevelProp &excluder : door_excluders)
		//props.push_back(excluder);

		std::vector<LevelPropInternal> spawned_props;
		if (!floor.prop_spawns.empty())
		{
			spawned_props = generate_props_from_spawns(floor, door_excluders);

			for (const LevelPropInternal &prop : spawned_props)
				props.push_back(prop);
		}

		if (!floor.skip_prop_generation)
		{
			const std::vector<LevelPropInternal> floorprops = generate_new_props(floor, door_excluders);

			for (const auto &prop : floorprops)
				props.push_back(prop);
		}

		// generate them lil transition strips between the floor textures
		const std::vector<LevelPropInternal> strips = generate_transition_strips(floor);
		for (const auto &strip : strips)
			props.push_back(strip);
	}

	return props;
}

// generate excluders around door openings
std::vector<LevelPropExcluder> LevelGenerator::generate_door_excluders(const LevelFloorInternal &floor)
{
	std::vector<LevelPropExcluder> excluders;

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

		excluders.emplace_back(x, y, w, h);
	}

	return excluders;
}

// generate some brand-new props for a given floor. randomly.
std::vector<LevelPropInternal> LevelGenerator::generate_new_props(const LevelFloorInternal &floor, const std::vector<LevelPropExcluder> &excluders)
{
	std::vector<LevelPropInternal> props;

	// generate some rugs
	if (rand.one_in(2))
	{
		const LevelPropDefinition propdef = PropDefinitions::get().rugs.at(rand.uniform_int(0, PropDefinitions::get().rugs.size() - 1));

		LevelSide sides[4] { LevelSide::left, LevelSide::right, LevelSide::bottom, LevelSide::top };
		const LevelSide orientation = sides[rand.uniform_int(0, 3)];

		const float x = (floor.x + (floor.w / 2.0f)) - (propdef.get_width(orientation) / 2.0f);
		const float y = (floor.y + (floor.h / 2.0f)) - (propdef.get_height(orientation) / 2.0f);

		const auto rug = propdef.spawn(orientation, x, y);

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
			const LevelPropDefinition &propdef = PropDefinitions::get().side_tables.at(rand.uniform_int(0, PropDefinitions::get().side_tables.size() - 1));
			const LevelSide side = random_side();
			const float table_margin = 0.075f;

			float x, y;

			switch (side)
			{
				case LevelSide::left:
					x = floor.x + table_margin;
					y = rand.uniform_real(floor.y, (floor.y + floor.h) - propdef.get_height(side));
					break;
				case LevelSide::right:
					x = ((floor.x + floor.w) - propdef.get_width(side)) - table_margin;
					y = rand.uniform_real(floor.y, (floor.y + floor.h) - propdef.get_height(side));
					break;
				case LevelSide::bottom:
					x = rand.uniform_real(floor.x, (floor.x + floor.w) - propdef.get_width(side));
					y = floor.y + table_margin;
					break;
				case LevelSide::top:
					x = rand.uniform_real(floor.x, (floor.x + floor.w) - propdef.get_width(side));
					y = ((floor.y + floor.h) - propdef.get_height(side)) - table_margin;
					break;
			}

			const auto prop = propdef.spawn(side, x, y);

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
		const LevelPropDefinition &propdef = PropDefinitions::get().center_tables.at(rand.uniform_int(0, PropDefinitions::get().center_tables.size() - 1));

		LevelSide sides[4] { LevelSide::left, LevelSide::right, LevelSide::bottom, LevelSide::top };
		const LevelSide orientation = sides[rand.uniform_int(0, 3)];

		const float x = (floor.x + (floor.w / 2.0f)) - (propdef.get_width(orientation) / 2.0f);
		const float y = (floor.y + (floor.h / 2.0f)) - (propdef.get_height(orientation) / 2.0f);

		const auto table = propdef.spawn(orientation, x, y);

		if (!table.collide(props) && !table.collide(excluders))
			props.push_back(table);
	}

	return props;
}

// some floors have pre-defined prop spawns. handle those
std::vector<LevelPropInternal> LevelGenerator::generate_props_from_spawns(const LevelFloorInternal &floor, const std::vector<LevelPropExcluder> &excluders)
{
	std::vector<LevelPropInternal> props;

	for (const auto &prop : floor.prop_spawns)
	{
		if (!prop.collide(props) && !prop.collide(excluders))
			props.push_back(prop);
	}

	return props;
}

// generate those lil dooly-bobs between floors
std::vector<LevelPropInternal> LevelGenerator::generate_transition_strips(const LevelFloorInternal &floor)
{
	std::vector<LevelPropInternal> props;

	const LevelPropDefinition &def = PropDefinitions::get().floor_transition_strip;

	for (const LevelFloorConnector &con : floor.connectors)
	{
		LevelSide o;

		float x = 0.0f, y = 0.0f;
		switch (con.side)
		{
			case LevelSide::left:
				o = LevelSide::left;
				x = floor.x - (def.get_width(o) / 2.0f);
				y = con.start;
				break;
			case LevelSide::right:
				continue;
			case LevelSide::bottom:
				o = LevelSide::bottom;
				x = con.start;
				y = floor.y - (def.get_height(o) / 2.0f);
				break;
			case LevelSide::top:
				continue;
		}

		LevelPropInternal strip = def.spawn(o, x, y);

		props.push_back(strip);
	}

	return props;
}

///////////////////////////////////////////
/// random helper functions
///////////////////////////////////////////

LevelSide LevelGenerator::flip(LevelSide side)
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

LevelSide LevelGenerator::random_side()
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

std::array<LevelSide, 4> LevelGenerator::enumerate_connector_sides()
{
	std::array<LevelSide, 4> sides;

	sides.at(0) = LevelSide::left;
	sides.at(1) = LevelSide::right;
	sides.at(2) = LevelSide::bottom;
	sides.at(3) = LevelSide::top;

	return sides;
}

std::vector<LevelFloorConnector> LevelGenerator::get_connectors(const std::vector<LevelFloorConnector> &connectors, LevelSide side)
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

bool LevelGenerator::float_equals(float a, float b, float tolerance)
{
	return std::fabs(a - b) < tolerance;
}

// append the seed to a hidden file
void LevelGenerator::log_seed(int seed)
{
#if defined WINPLAT_WINDOWS
	const char *local_app_data = std::getenv("LOCALAPPDATA");
	const auto basepath = local_app_data ? std::filesystem::path(local_app_data) / ".darktimes-seeds.txt" : "";
#elif defined WINPLAT_LINUX
	const char *home = std::getenv("HOME");
	const auto basepath = home ? std::filesystem::path(home) / ".darktimes-seeds.txt" : "";
#endif
	char timestring[100];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(timestring, sizeof(timestring), "%Y-%m-%d %I:%M:%S %p", t);

	std::ofstream out(basepath, std::ofstream::app);
	if (out)
		out << timestring << "\t" << seed << std::endl;
}
