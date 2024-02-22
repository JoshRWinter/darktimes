#pragma once

#include <vector>

#include "../Texture.hpp"

enum class LevelSide
{
	left,
	right,
	bottom,
	top
};

struct LevelProp
{
	LevelProp(Texture texture, LevelSide side, float x, float y, float w, float h)
		: texture(texture)
		, side(side)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
	{}

	Texture texture;
	LevelSide side;
	float x, y, w, h;
};

struct LevelWall
{
	LevelWall(float x, float y, float w, float h)
		: x(x)
		, y(y)
		, w(w)
		, h(h)
	{}

	float x, y, w, h;
};

struct LevelFloor
{
	LevelFloor(Texture texture, float x, float y, float w, float h)
		: texture(texture)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
	{}

	Texture texture;
	float x, y, w, h;
};
