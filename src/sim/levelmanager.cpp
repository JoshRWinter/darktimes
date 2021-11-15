#include <cmath>
#include <stack>

#include "../darktimes.hpp"
#include "../assetmanager.hpp"
#include "levelmanager.hpp"

static std::vector<LevelWall> generate_walls(const LevelFloor &floor)
{
	std::vector<LevelWall> walls;

	walls.emplace_back(floor.x, floor.y, floor.x + floor.w, floor.y);
	walls.emplace_back(floor.x, floor.y, floor.x, floor.y + floor.h);
	walls.emplace_back(floor.x, floor.y + floor.h, floor.x + floor.w, floor.y + floor.h);
	walls.emplace_back(floor.x + floor.w, floor.y + floor.h, floor.x + floor.w, floor.y);

	return walls;
}

LevelManager::LevelManager(int seed)
	: mersenne(seed)
{
	reset();
}

void LevelManager::reset()
{
	floors.clear();

	floors.emplace_back(random_int(0, 1), -1.0f, -1.0f, 3.0f, 3.0f);

	for (int i = 0; i < 100; ++i)
	{
		connect_new(floors, floors.size() - 1);
	}

	const int floorcount = floors.size();
	for (int i = 0; i < floorcount; ++i)
	{
		if (random_int(0, 10) > 2)
			continue;

		const int count = random_int(1, 10);
		if (connect_new(floors, i))
		{
			for (int j = 0; j < count; ++j)
			{
				if (!connect_new(floors, floors.size() - 1))
					break;
			}
		}
	}

	/*
	for (const auto &floor : floors)
	{
		for (const auto &wall : generate_walls(floor))
			walls.push_back(wall);
	}
	*/
}

int LevelManager::random_int(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(mersenne);
}

float LevelManager::random_real(float low, float high)
{
	return std::uniform_real_distribution<float>(low, high)(mersenne);
}

bool LevelManager::connect_new(std::vector<LevelFloor> &floors, int index)
{
	auto &floor = floors.at(index);

	const float overlap = 0.8f;

	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

	if (random_int(0, 1)) // squarish
	{
		w = random_real(1.0f, 2.0f);
		h = random_real(1.0f, 2.0f);
	}
	else // long boi
	{
		if (random_int(0, 1)) // horizontal
		{
			w = random_real(3.0f, 7.0f);
			h = random_real(2.0f, 3.0f);
		}
		else // vertical
		{
			w = random_real(2.0f, 3.0f);
			h = random_real(3.0f, 7.0f);
		}
	}

	enum class Side
	{
		left, right, bottom, top
	} side;

	int tries = 100;
	while (tries--)
	{
		if (random_int(0, 1)) // left/right
		{
			if (random_int(0, 1)) // left
			{
				side = Side::left;
				x = floor.x + floor.w;
			}
			else // right
			{
				side = Side::right;
				x = floor.x - w;
			}

			const float low = (floor.y - h) + overlap;
			const float high = floor.y + (floor.h - overlap);

			y = random_real(low, high);
		}
		else // bottom/top
		{
			if (random_int(0, 1)) // bottom
			{
				side = Side::bottom;
				y = floor.y - h;
			}
			else // top
			{
				side = Side::top;
				y = floor.y + floor.h;
			}

			const float left = (floor.x - w) + overlap;
			const float right = floor.x + (floor.w - overlap);

			x = random_real(left, right);
		}

		LevelFloor newfloor(random_int(0, 1), x, y, w, h);

		if (!test_floor(floors, newfloor))
			continue;

		switch (side)
		{
		case Side::left:
			floor.rel_left.push_back(floors.size());
			newfloor.rel_right.push_back(index);
			break;
		case Side::right:
			floor.rel_right.push_back(floors.size());
			newfloor.rel_left.push_back(index);
			break;
		case Side::bottom:
			floor.rel_bottom.push_back(floors.size());
			newfloor.rel_top.push_back(index);
			break;
		case Side::top:
			floor.rel_top.push_back(floors.size());
			newfloor.rel_bottom.push_back(index);
			break;
		}

		floors.push_back(newfloor);

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
