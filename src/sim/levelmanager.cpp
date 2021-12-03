#include <cmath>
#include <stack>

#include "../darktimes.hpp"
#include "../assetmanager.hpp"
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

	sides.at(0) = LevelSide::LEFT;
	sides.at(1) = LevelSide::RIGHT;
	sides.at(2) = LevelSide::BOTTOM;
	sides.at(3) = LevelSide::TOP;

	return sides;
}

static LevelSide flip(LevelSide side)
{
	switch (side)
	{
	case LevelSide::LEFT:
		return LevelSide::RIGHT;
	case LevelSide::RIGHT:
		return LevelSide::LEFT;
	case LevelSide::BOTTOM:
		return LevelSide::TOP;
	case LevelSide::TOP:
		return LevelSide::BOTTOM;
	}

	win::bug("no side");
}

LevelManager::LevelManager(int seed)
	: mersenne(seed)
	, seed(seed)
{
	generate();
}

void LevelManager::generate()
{
	reset();

	while (!generate_grid()) reset();

	prune();

	generate_walls();
}

void LevelManager::reset()
{
	floors.clear();
	walls.clear();
	floors.emplace_back(random_int(0, 1), -1.0f, -1.0f, 2.0f, 2.0f);
}

void LevelManager::prune()
{
	for (auto floor = floors.begin(); floor != floors.end();)
	{
		if (floor->connectors.empty())
		{
			floor = floors.erase(floor);
			continue;
		}

		++floor;
	}
}

bool LevelManager::generate_grid()
{
	const int horizontal_tiles = random_int(3, 6);
	const int vertical_tiles = random_int(3, 6);
	const float tile_width = random_real(2.6f, 3.5f);
	const float tile_height = random_real(2.6f, 3.5f);

	//LevelFloor *head = &floors.at(0);

	float start_x = -4.0f;
	float start_y = floors.at(0).y + floors.at(0).h;
	const float initial_start_y = start_y;

	int floors_added = 0;
	for (int i = 0; i < horizontal_tiles; ++i)
	{
		for (int j = 0; j < vertical_tiles; ++j)
		{
			const bool long_horizontal = random_int(0, 6) == 0;
			const bool long_vertical = random_int(0, 6) == 0;

			LevelFloor floor(random_int(0, 1), start_x, start_y, long_horizontal ? tile_width * 2.0f : tile_width, long_vertical ? tile_height * 2.0f : tile_height);

			if (floor.collide(floors))
				continue;

			floors.push_back(floor);
			++floors_added;

			start_y += floors.at(floors.size() - 1).h;
		}

		start_y = initial_start_y;
		start_x += floors.at(floors.size() - 1).w;
	}

	auto head = find_neighbor(floors.at(0), LevelSide::TOP);
	if (head == NULL) return false;
	if (!connect(floors.at(0), *head)) return false;
	LevelSide camefrom = LevelSide::BOTTOM;

	std::stack<LevelFloor*> heads;
	heads.push(head);

	for (int i = 0; i < floors_added; ++i)
	{
		head = heads.top();
		bool success = false;

		for (int attempt = 0; attempt < 2; ++attempt)
		{
			LevelSide dir = camefrom;
			while (dir == camefrom)
				dir = random_side();

			LevelFloor *const partner = find_neighbor(*head, dir);
			if (partner == NULL)
				continue;

			if (connect(*head, *partner))
			{
				heads.push(partner);
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

	return true;
}

bool LevelManager::generate_linear()
{
	return false;
}

LevelFloor *LevelManager::find_neighbor(LevelFloor& floor, LevelSide side)
{
	for (auto &f : floors)
	{
		if (&floor == &f)
			continue;

		LevelFloorConnector c1;
		LevelFloorConnector c2;

		if (can_connect(floor, f, c1, c2))
		{
			if (c1.side == side)
				return &f;
		}
	}

	return NULL;
}

void LevelManager::generate_walls()
{
	for (const auto &floor : floors)
	{
		for (const auto side : enumerate_connector_sides())
		{
			auto connectors	= get_connectors(floor.connectors, side);

			if (side == LevelSide::BOTTOM || side == LevelSide::TOP)
			{
				const float y = side == LevelSide::BOTTOM ? floor.y - LevelWall::HALFWIDTH : ((floor.y + floor.h) - LevelWall::HALFWIDTH);
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
				const float x = side == LevelSide::LEFT ? floor.x - LevelWall::HALFWIDTH : ((floor.x + floor.w) - LevelWall::HALFWIDTH);
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

		/*
		walls.emplace_back(LevelWall::Side::BOTTOM, floor.x, floor.y, floor.x + floor.w, floor.y);
		walls.emplace_back(LevelWall::Side::LEFT, floor.x, floor.y, floor.x, floor.y + floor.h);
		walls.emplace_back(LevelWall::Side::TOP, floor.x, floor.y + floor.h, floor.x + floor.w, floor.y + floor.h);
		walls.emplace_back(LevelWall::Side::RIGHT, floor.x + floor.w, floor.y, floor.x + floor.w, floor.y + floor.h);
		*/
	}
}

int LevelManager::random_int(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(mersenne);
}

float LevelManager::random_real(float low, float high)
{
	return std::uniform_real_distribution<float>(low, high)(mersenne);
}

LevelSide LevelManager::random_side()
{
	const int i = random_int(0, 3);

	switch (i)
	{
	case 0:
		return LevelSide::LEFT;
	case 1:
		return LevelSide::RIGHT;
	case 2:
		return LevelSide::BOTTOM;
	case 3:
		return LevelSide::TOP;
	}

	win::bug("no side");
}

bool LevelManager::can_connect(LevelFloor &floor1, LevelFloor &floor2, LevelFloorConnector &c1, LevelFloorConnector &c2)
{
	// determine which side floor 2 is on, relative to floor 1
	LevelSide side;
	if (float_equals(floor1.x + floor1.w, floor2.x)) // right?
		side = LevelSide::RIGHT;
	else if (float_equals(floor1.x, floor2.x + floor2.w)) // left?
		side = LevelSide::LEFT;
	else if (float_equals(floor1.y, floor2.y + floor2.h)) // bottom?
		side = LevelSide::BOTTOM;
	else if (float_equals(floor1.y + floor1.h, floor2.y)) // top?
		side = LevelSide::TOP;
	else
		return false;

	const float MIN_WALL_LENGTH = 1.8f; // minimum wall length needed to support a door (with some padding)
	const float DOOR_LENGTH = 1.0f;

	float start, stop;
	if (side == LevelSide::BOTTOM || side == LevelSide::TOP)
	{
		const float farleft = std::max(floor1.x, floor2.x);
		const float farright = std::min(floor1.x + floor1.w, floor2.x + floor2.w);
		if (farright - farleft < MIN_WALL_LENGTH)
			return false;

		start = ((farright + farleft) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}
	else if (side == LevelSide::LEFT || side == LevelSide::RIGHT)
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
	case LevelSide::TOP:
		flipside = LevelSide::BOTTOM;
		break;
	case LevelSide::BOTTOM:
		flipside = LevelSide::TOP;
		break;
	case LevelSide::LEFT:
		flipside = LevelSide::RIGHT;
		break;
	case LevelSide::RIGHT:
		flipside = LevelSide::LEFT;
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
