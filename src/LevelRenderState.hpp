#pragma once

#include <vector>

#include "Darktimes.hpp"
#include "Renderable.hpp"

struct LevelRenderState
{
	NO_COPY_MOVE(LevelRenderState);

	LevelRenderState() { reset(); }

	std::vector<Renderable> renderables;
	int seed;

	void reset()
	{
		renderables.clear();
		seed = -1;
	}
};
