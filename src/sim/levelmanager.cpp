#include <cmath>

#include "../darktimes.hpp"
#include "levelmanager.hpp"

LevelManager::LevelManager()
{
	reset();
}

void LevelManager::reset()
{
	walls.clear();

	walls.emplace_back(0.0f, 0.0f, 1.0f, 0.0f);
	walls.emplace_back(0.0f, 1.0f, 0.0f, 0.0f);
	walls.emplace_back(0.0f, 1.0f, 1.0f, 1.0f);
	walls.emplace_back(1.0f, 1.0f, 1.0f, 0.0f);
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
