#pragma once

struct Renderable
{
	Renderable(int texture, float x, float y, float w, float h, float rot)
		: texture(texture)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
		, rot(rot)
	{}

	int texture;
	float x;
	float y;
	float w;
	float h;
	float rot;
};
