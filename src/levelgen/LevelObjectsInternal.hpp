#pragma once

#include "LevelObjects.hpp"

enum class LevelSide
{
	left,
	right,
	bottom,
	top
};

struct PropExcluder
{
	PropExcluder(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h) {}

	float x, y, w, h;
};

struct LevelPropInternal : LevelProp
{
	LevelPropInternal(Texture texture, LevelPropOrientation orientation, int collision_class, int collides_with_class, float x, float y, float w, float h, float excluder_padding_y, float excluder_padding_x)
		: LevelProp(texture, orientation, x, y, w, h)
		, collision_class(collision_class)
		, collides_with_class(collides_with_class)
		, excluder_padding_x(excluder_padding_x)
		, excluder_padding_y(excluder_padding_y)
	{}

	bool collide(const std::vector<LevelPropInternal> &others) const
	{
		for (const auto &other : others)
		{
			if (collide(other))
				return true;
		}

		return false;
	}

	bool collide(const LevelPropInternal &other) const
	{
		return matches_collision_class(other.collision_class) &&
			x + w + excluder_padding_x > other.x - other.excluder_padding_x &&
			x - excluder_padding_x < other.x + other.w + other.excluder_padding_x &&
			y + h + excluder_padding_y > other.y - other.excluder_padding_y &&
			y - excluder_padding_y < other.y + other.h + other.excluder_padding_y;
	}

	bool collide(const std::vector<PropExcluder> &others) const
	{
		for (const auto &other : others)
		{
			if (collide(other))
				return true;
		}

		return false;
	}

	bool collide(const PropExcluder &other) const
	{
		return x + w + excluder_padding_x > other.x &&
			   x - excluder_padding_x < other.x + other.w &&
			   y + h + excluder_padding_y > other.y &&
			   y - excluder_padding_y < other.y + other.h;
	}

	bool matches_collision_class(int other_collision_class) const
	{
		// loop over all the bits
		for (int position = 0; position < sizeof(collides_with_class) * 8; ++position)
		{
			bool left_bit = ((collides_with_class >> position) & 1) == 1;
			bool right_bit = ((other_collision_class >> position) & 1) == 1;

			if (left_bit && right_bit)
				return true;
		}

		return false;
	}

	int collision_class;
	int collides_with_class;
	float excluder_padding_x, excluder_padding_y;
};

struct LevelFloorConnector
{
	LevelFloorConnector() = default;

	LevelFloorConnector(LevelSide side, float start, float stop)
		: side(side)
		, start(start)
		, stop(stop)
	{}

	bool collide(const LevelFloorConnector &rhs, float padding) const
	{
		if (side != rhs.side)
			return false;

		const bool above = rhs.start > stop + padding;
		const bool below = rhs.stop < start - padding;

		if (above || below)
			return false;

		return true;
	}

	bool collide(const std::vector<LevelFloorConnector> &rhs, float padding) const
	{
		for (const auto &x : rhs)
			if (collide(x, padding))
				return true;

		return false;
	}

	LevelSide side;
	float start, stop;
};

struct LevelWallInternal : LevelWall
{
	static constexpr float HALFWIDTH = 0.05f;
	static constexpr float WIDTH = HALFWIDTH * 2;

	LevelWallInternal(float x, float y, float w, float h)
		: LevelWall(x, y, w, h)
	{}
};

struct LevelFloorInternal : LevelFloor
{
	LevelFloorInternal(Texture texture, float x, float y, float w, float h, bool skip_prop_generation = false)
		: LevelFloor(texture, x, y, w, h)
		, skip_prop_generation(skip_prop_generation)
	{}

	bool collide(const LevelFloorInternal &rhs, float tolerance = 0.05f) const { return x + w > rhs.x + tolerance && x < (rhs.x + rhs.w) - tolerance && y + h > rhs.y + tolerance && y < (rhs.y + rhs.h) - tolerance; }
	bool collide(const std::vector<LevelFloorInternal> &rhs) const
	{
		for (const auto &x : rhs)
			if (collide(x))
				return true;

		return false;
	}

	bool skip_prop_generation;
	std::vector<LevelFloorConnector> connectors;
	std::vector<LevelPropInternal> prop_spawns;
};
