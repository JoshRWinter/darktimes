#include <cmath>
#include <stack>

#include "../darktimes.hpp"
#include "../assetmanager.hpp"
#include "levelmanager.hpp"

static bool float_equals(float a, float b, float tolerance = 0.005f)
{
	return std::fabs(a - b) < tolerance;
}

static std::vector<LevelFloorConnector> get_connectors(const std::vector<LevelFloorConnector> &connectors, LevelFloorConnector::Side side)
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

static std::array<LevelFloorConnector::Side, 4> enumerate_connector_sides()
{
	std::array<LevelFloorConnector::Side, 4> sides;

	sides.at(0) = LevelFloorConnector::Side::LEFT;
	sides.at(1) = LevelFloorConnector::Side::RIGHT;
	sides.at(2) = LevelFloorConnector::Side::BOTTOM;
	sides.at(3) = LevelFloorConnector::Side::TOP;

	return sides;
}

LevelManager::LevelManager(int seed)
	: mersenne(seed)
{
	reset();
}

void LevelManager::reset()
{
	floors.emplace_back(random_int(0, 1), -1.0f, -1.0f, 2.0f, 2.0f);

	generate_grid();

	generate_walls();
}

void LevelManager::generate_grid()
{
	const int horizontal_tiles = 5;random_int(3, 6);
	const int vertical_tiles = 5;random_int(3, 6);
	const float tile_width = 2.8f;
	const float tile_height = 2.8f;

	LevelFloor *head = &floors.at(0);

	float start_x = -4.0f;
	float start_y = head->y + head->h;
	const float initial_start_y = start_y;

	for (int i = 0; i < horizontal_tiles; ++i)
	{
		for (int j = 0; j < vertical_tiles; ++j)
		{
			const bool long_horizontal = random_int(0, 6) == 0;
			const bool long_vertical = random_int(0, 6) == 0;

			LevelFloor floor(random_int(0, 1), start_x, start_y, long_horizontal ? tile_width * 2.0f : tile_width, long_vertical ? tile_height * 2.0f : tile_height);

			/*
			if (!floor.collide(floors))
				win::bug("floor collision!");

			if (!connect(floor, *head))
				win::bug("no connection");
				*/

			floors.push_back(floor);
			head = &floors.at(floors.size() - 1);

			/*
			if (long_horizontal)
				++i;
			if (long_vertical)
				++j;
				*/

			start_y += head->h;
		}
		start_y = initial_start_y;
		start_x += head->w;
	}
}

void LevelManager::generate_linear()
{
}

void LevelManager::generate_walls()
{
	for (const auto &floor : floors)
	{
		for (const auto side : enumerate_connector_sides())
		{
			auto connectors	= get_connectors(floor.connectors, side);

			if (side == LevelFloorConnector::Side::BOTTOM || side == LevelFloorConnector::Side::TOP)
			{
				const float y = side == LevelFloorConnector::Side::BOTTOM ? floor.y - LevelWall::HALFWIDTH : ((floor.y + floor.h) - LevelWall::HALFWIDTH);
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
				const float x = side == LevelFloorConnector::Side::LEFT ? floor.x - LevelWall::HALFWIDTH : ((floor.x + floor.w) - LevelWall::HALFWIDTH);
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

bool LevelManager::connect(LevelFloor &floor1, LevelFloor &floor2)
{
	// determine which side floor 2 is on, relative to floor 1
	LevelFloorConnector::Side side;
	if (float_equals(floor1.x + floor1.w, floor2.x)) // right?
		side = LevelFloorConnector::Side::RIGHT;
	else if (float_equals(floor1.x, floor2.x + floor2.w)) // left?
		side = LevelFloorConnector::Side::LEFT;
	else if (float_equals(floor1.y, floor2.y + floor2.h)) // bottom?
		side = LevelFloorConnector::Side::BOTTOM;
	else if (float_equals(floor1.y + floor1.h, floor2.y)) // top?
		side = LevelFloorConnector::Side::TOP;
	else
		return false;

	const float MIN_WALL_LENGTH = 1.8f; // minimum wall length needed to support a door (with some padding)
	const float DOOR_LENGTH = 1.0f;

	float start, stop;
	if (side == LevelFloorConnector::Side::BOTTOM || side == LevelFloorConnector::Side::TOP)
	{
		const float farleft = std::max(floor1.x, floor2.x);
		const float farright = std::min(floor1.x + floor1.w, floor2.x + floor2.w);
		if (farright - farleft < MIN_WALL_LENGTH)
			return false;

		start = ((farright + farleft) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}
	else if (side == LevelFloorConnector::Side::LEFT || side == LevelFloorConnector::Side::RIGHT)
	{
		const float lower = std::max(floor1.y, floor2.y);
		const float upper = std::min(floor1.y + floor1.h, floor2.y + floor2.h);

		if (upper - lower < MIN_WALL_LENGTH)
			return false;

		start = ((upper + lower) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}

	LevelFloorConnector::Side flipside;
	switch (side)
	{
	case LevelFloorConnector::Side::TOP:
		flipside = LevelFloorConnector::Side::BOTTOM;
		break;
	case LevelFloorConnector::Side::BOTTOM:
		flipside = LevelFloorConnector::Side::TOP;
		break;
	case LevelFloorConnector::Side::LEFT:
		flipside = LevelFloorConnector::Side::RIGHT;
		break;
	case LevelFloorConnector::Side::RIGHT:
		flipside = LevelFloorConnector::Side::LEFT;
		break;
	}

	LevelFloorConnector floor1_connector(&floor2, side, start, stop);
	LevelFloorConnector floor2_connector(&floor1, flipside, start, stop);

	if (floor1_connector.collide(floor1.connectors, 0.5f))
		return false;
	if (floor2_connector.collide(floor2.connectors, 0.5f))
		return false;

	floor1.connectors.push_back(floor1_connector);
	floor2.connectors.push_back(floor2_connector);

	return true;
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
