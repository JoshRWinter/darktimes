#pragma once

#include <win/Utility.hpp>

struct LightRenderable
{
	LightRenderable(float x, float y, float radius, const win::Color<float> &color) : x(x), y(y), radius(radius), color(color) {}

	float x, y;
	float radius;
	win::Color<float> color;
};
