#include <cmath>

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

LevelManager::LevelManager()
	: mersenne(time(NULL))
{
	reset();
}

void LevelManager::reset()
{
	walls.clear();
	floors.clear();

	floors.emplace_back(random_int(0, 1), -1.0f, -1.0f, 3.0f, 3.0f);

	for (const auto &floor : floors)
	{
		for (const auto &wall : generate_walls(floor))
			walls.push_back(wall);
	}
}

int LevelManager::random_int(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(mersenne);
}
