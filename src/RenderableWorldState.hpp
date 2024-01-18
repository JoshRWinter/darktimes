#pragma once

#include <vector>

#include "Renderable.hpp"

struct RenderableWorldState
{
	std::vector<Renderable> renderables;
	float centerx = 0.0f;
	float centery = 0.0f;
};
