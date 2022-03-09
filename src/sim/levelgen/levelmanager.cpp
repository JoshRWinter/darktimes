#include <cmath>
#include <stack>
#include <algorithm>

#include "../../render/texture.hpp"
#include "propdefs.hpp"
#include "structuredefs.hpp"
#include "levelmanager.hpp"

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

LevelManager::LevelManager(int seed)
	: rand(seed)
	, seed(seed)
{
	fprintf(stderr, "%d\n", seed);
	generate();
}

void LevelManager::generate()
{
	reset();

	while (!generate_impl()) reset();

	generate_walls();
	generate_props();
}

bool LevelManager::generate_impl()
{
	floors.emplace_back(0, -1.0f, -1.0f, 2.0f, 2.0f);

	/*
	// ========= structure testing
	for (const auto &floor : generate_structure(floors.at(0), LevelSide::top, 2))
		floors.push_back(floor);
	connect(floors.at(0), floors.at(1));
	return true;
	// ========= structure testing
	*/

	bool first = true;
	while (floors.size() < 50)
	{
		std::vector<LevelFloor> generated;
		int attempts = 0;
		int start_candidate_index;
		LevelSide side = first ? LevelSide::top : random_side();
		do
		{
			start_candidate_index = find_start_candidate(floors, attempts++);

			if (start_candidate_index == -1)
				return false;

			generated = prune(generate_grid(floors.at(start_candidate_index), side));
		} while (generated.size() < 5);

		connect(floors.at(start_candidate_index), *find_neighbors(generated, floors.at(start_candidate_index), side).at(0));

		for (const auto &floor : generated)
			floors.push_back(floor);

		first = false;

		if (rand.one_in(2) || true)
		{
			attempts = 0;
			do
			{
				side = random_side();
				start_candidate_index = find_start_candidate(floors, attempts++);

				if (start_candidate_index == -1)
					return false;

				generated = generate_structure(floors.at(start_candidate_index), side);
			} while (generated.size() < 1);

			connect(floors.at(start_candidate_index), *find_neighbors(generated, floors.at(start_candidate_index), side).at(0));

			for (const auto &floor : generated)
				floors.push_back(floor);
		}
		else
		{
			attempts = 0;
			do
			{
				side = random_side();
				start_candidate_index = find_start_candidate(floors, attempts++);

				if (start_candidate_index == -1)
					return false;

				generated = generate_linear(floors.at(start_candidate_index), side);
			} while (generated.size() < 2);

			connect(floors.at(start_candidate_index), *find_neighbors(generated, floors.at(start_candidate_index), side).at(0));

			for (const auto &floor : generated)
				floors.push_back(floor);
		}
	}

	return true;
}

int LevelManager::find_start_candidate(const std::vector<LevelFloor> &floors, const int attempt_num)
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

void LevelManager::reset()
{
	floors.clear();
	walls.clear();
}

std::vector<LevelFloor> LevelManager::prune(const std::vector<LevelFloor> &floors)
{
	std::vector<LevelFloor> pruned;

	for (const auto &floor : floors)
	{
		if (!floor.connectors.empty())
			pruned.push_back(floor);
	}

	return pruned;
}

std::vector<LevelFloor> LevelManager::generate_grid(const LevelFloor &start_floor, LevelSide start_side)
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
	}else if (start_side == LevelSide::left)
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
			if (floor.collide(floors))
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
				dir = random_side();

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

std::vector<LevelFloor> LevelManager::generate_linear(const LevelFloor &start_floor, const LevelSide start_side)
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
			const LevelSide side = head == NULL ? start_side : random_side();

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

			if (floor.collide(floors))
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

std::vector<LevelFloor> LevelManager::generate_structure(const LevelFloor &start_floor, const LevelSide start_side, int force_index)
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

		if (floor.collide(floors))
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

std::vector<LevelFloor*> LevelManager::find_neighbors(std::vector<LevelFloor> &floors, const LevelFloor& floor, LevelSide side)
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

void LevelManager::generate_walls()
{
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
}

void LevelManager::generate_props()
{
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

			const std::vector<LevelProp> floorprops = generate_new_props(floor, all_excluders);

			for (const LevelProp &prop : floorprops)
				props.push_back(prop);
		}
	}
}

std::vector<LevelProp> LevelManager::generate_new_props(const LevelFloor &floor, const std::vector<LevelProp> &excluders)
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

		if (width_pct < 0.8f && height_pct < 0.8f)
			props.push_back(rug);
	}

	// generate some side tables
	const int side_tables = rand.uniform_int(0, 4);
	for (int i = 0; i < side_tables; ++i)
	{
		const int attempts = 3;

		for (int j = 0; j < attempts; ++j)
		{
			const LevelPropDefinition &propdef = PropDefinitions::side_tables.at(rand.uniform_int(0, PropDefinitions::side_tables.size() - 1));
			const LevelSide side = random_side();
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

std::vector<LevelProp> LevelManager::generate_props_from_spawns(const LevelFloor &floor, const std::vector<LevelProp> &excluders)
{
	std::vector<LevelProp> props;

	for (const LevelProp &prop : floor.prop_spawns)
	{
		if (!prop.collide(props) && !prop.collide(excluders))
			props.push_back(prop);
	}

	return props;
}

std::vector<LevelProp> LevelManager::generate_door_excluders(const LevelFloor &floor)
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

		excluders.emplace_back(LevelPropOrientation::left, LevelPropDefinition(0, true, w, h, 0.1f, 0.1f), x, y);
	}

	return excluders;
}

LevelSide LevelManager::random_side()
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
	}

	win::bug("no side");
}

bool LevelManager::can_connect(const LevelFloor &floor1, const LevelFloor &floor2, LevelFloorConnector &c1, LevelFloorConnector &c2)
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

bool LevelManager::connect(LevelFloor &floor1, LevelFloor &floor2)
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

bool LevelManager::test_floor(const std::vector<LevelFloor> &floors, const LevelFloor &floor)
{
	for (const auto &f : floors)
	{
		if (f.collide(floor))
			return false;
	}

	return true;
}
