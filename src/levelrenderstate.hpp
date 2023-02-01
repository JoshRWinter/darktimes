#pragma once

#include <vector>

#include "darktimes.hpp"
#include "render/renderable.hpp"

struct LevelRenderState
{
	NO_COPY_MOVE(LevelRenderState);

	LevelRenderState() { reset(); }

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
