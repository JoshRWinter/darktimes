#pragma once

#include "Texture.hpp"

struct Renderable
{
	Renderable(Texture texture, float x, float y, float w, float h, float rot)
		: texture(texture)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
		, rot(rot)
	{}

	Texture texture;
	float x;
	float y;
	float w;
	float h;
	float rot;
};
