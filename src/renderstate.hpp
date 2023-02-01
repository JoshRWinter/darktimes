#pragma once

#include <vector>

#include "darktimes.hpp"
#include "render/renderable.hpp"

struct RenderState
{
	NO_COPY_MOVE(RenderState);

	RenderState()
		: centerx(0.0f)
		, centery(0.0f)
	{}

	float centerx, centery;
	std::vector<Renderable> renderables;
};
