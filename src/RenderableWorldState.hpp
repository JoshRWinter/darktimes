#pragma once

#include <vector>

#include "LightRenderable.hpp"
#include "Renderable.hpp"

struct RenderableWorldState
{
	std::vector<Renderable> dynamics;
	std::vector<LightRenderable> dynamic_lights;
	float centerx = 0.0f;
	float centery = 0.0f;
};
