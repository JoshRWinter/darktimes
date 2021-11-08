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

	floors.emplace_back(-1.0f, -1.0f, 3.0f, 3.0f);

	for (const auto &floor : floors)
	{
		for (const auto &wall : generate_walls(floor))
			walls.push_back(wall);
	}
}

std::vector<float> LevelManager::get_floor_verts()
{
	std::vector<float> verts;

	for (const auto &floor : floors)
	{
		const float x_magnitude = floor.w / AssetManager::floor_texture_tile_size;
		const float y_magnitude = floor.h / AssetManager::floor_texture_tile_size;

		const float texture = random_int(0, 1);

		verts.push_back(floor.x); verts.push_back(floor.y + floor.h); verts.push_back(0.0f); verts.push_back(y_magnitude); verts.push_back(texture);
		verts.push_back(floor.x); verts.push_back(floor.y); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y); verts.push_back(x_magnitude); verts.push_back(0.0f); verts.push_back(texture);

		verts.push_back(floor.x); verts.push_back(floor.y + floor.h); verts.push_back(0.0f); verts.push_back(y_magnitude); verts.push_back(texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y); verts.push_back(x_magnitude); verts.push_back(0.0f); verts.push_back(texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y + floor.h); verts.push_back(x_magnitude); verts.push_back(y_magnitude); verts.push_back(texture);
	}

	return verts;
}

std::vector<float> LevelManager::get_wall_verts() const
{
	std::vector<float> verts;

	for (const auto &line : walls)
	{
		if (line.ax == line.bx && line.ay == line.by)
			bug("invalid line");

		// determine orientation
		const float xdist = std::fabs(line.ax - line.bx);
		const float ydist = std::fabs(line.ay - line.by);

		const bool horizontal = xdist > ydist ? true : false;

		if (horizontal)
		{
			const float leftpoint_x = std::min(line.ax, line.bx);
			const float leftpoint_y = leftpoint_x == line.ax ? line.ay : line.by;
			const float rightpoint_x = std::max(line.ax, line.bx);
			const float rightpoint_y = rightpoint_x == line.ax ? line.ay : line.by;

			verts.push_back(leftpoint_x); verts.push_back(leftpoint_y + LevelWall::HALFWIDTH);
			verts.push_back(leftpoint_x); verts.push_back(leftpoint_y - LevelWall::HALFWIDTH);
			verts.push_back(rightpoint_x); verts.push_back(rightpoint_y - LevelWall::HALFWIDTH);

			verts.push_back(leftpoint_x); verts.push_back(leftpoint_y + LevelWall::HALFWIDTH);
			verts.push_back(rightpoint_x); verts.push_back(rightpoint_y - LevelWall::HALFWIDTH);
			verts.push_back(rightpoint_x); verts.push_back(rightpoint_y + LevelWall::HALFWIDTH);
		}
		else
		{
			const float upperpoint_y = std::max(line.ay, line.by);
			const float upperpoint_x = upperpoint_y == line.ay ? line.ax : line.bx;
			const float bottompoint_y = std::min(line.ay, line.by);
			const float bottompoint_x = bottompoint_y == line.ay ? line.ax : line.bx;

			verts.push_back(upperpoint_x - LevelWall::HALFWIDTH); verts.push_back(upperpoint_y);
			verts.push_back(bottompoint_x - LevelWall::HALFWIDTH); verts.push_back(bottompoint_y);
			verts.push_back(bottompoint_x + LevelWall::HALFWIDTH); verts.push_back(bottompoint_y);

			verts.push_back(upperpoint_x - LevelWall::HALFWIDTH); verts.push_back(upperpoint_y);
			verts.push_back(bottompoint_x + LevelWall::HALFWIDTH); verts.push_back(bottompoint_y);
			verts.push_back(upperpoint_x + LevelWall::HALFWIDTH); verts.push_back(upperpoint_y);
		}
	}

	return verts;
}

int LevelManager::random_int(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(mersenne);
}
