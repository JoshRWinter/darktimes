#pragma once

#include <vector>

#include "Darktimes.hpp"
#include "render/Renderable.hpp"

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
