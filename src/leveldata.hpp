#pragma once

#include <vector>

#include "render/renderable.hpp"

struct LevelData
{
	std::vector<Renderable> atlas_renderables;
	std::vector<Renderable> tile_renderables;
	int seed;

	void reset()
	{
		atlas_renderables.clear();
		tile_renderables.clear();
		seed = -1;
	}
};
