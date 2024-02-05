#pragma once

#include <vector>

#include "../Texture.hpp"

enum class LevelPropOrientation
{
	left,
	down,
	right,
	up
};

struct LevelProp
{
	LevelProp(Texture texture, LevelPropOrientation orientation, float x, float y, float w, float h)
		: texture(texture)
		, orientation(orientation)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
	{}

	Texture texture;
	LevelPropOrientation orientation;
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
