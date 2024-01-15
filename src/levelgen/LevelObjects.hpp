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

enum class LevelPropOrientation
{
	left,
	down,
	right,
	up
};

struct LevelPropDefinition
{
	static constexpr float default_excluder_padding = 0.2f;

	// bitflags
	static constexpr int collision_class_none = 0;
	static constexpr int collision_class_excluder = 1;
	static constexpr int collision_class_furniture = 2;
	static constexpr int collision_class_rug = 4;
	static constexpr int collision_class_debris = 8;

	LevelPropDefinition(int texture, int collision_class, int collides_with_class, float width, float height, float excluder_padding_x = default_excluder_padding, float excluder_padding_y = default_excluder_padding)
		: texture(texture), collision_class(collision_class), collides_with_class(collides_with_class), excluder_padding_x(excluder_padding_x), excluder_padding_y(excluder_padding_y), width(width), height(height)
	{}

	float get_width(LevelPropOrientation o) const { return o == LevelPropOrientation::left || o == LevelPropOrientation::right ? width : height; }
	float get_height(LevelPropOrientation o) const { return o == LevelPropOrientation::left || o == LevelPropOrientation::right ? height : width; }
	float get_excluder_padding_x(LevelPropOrientation o) const { return o == LevelPropOrientation::left || o == LevelPropOrientation::right ? excluder_padding_x : excluder_padding_y; }
	float get_excluder_padding_y(LevelPropOrientation o) const { return o == LevelPropOrientation::left || o == LevelPropOrientation::right ? excluder_padding_y : excluder_padding_x; }

	int texture;
	int collision_class;
	int collides_with_class;

private:
	float excluder_padding_x;
	float excluder_padding_y;
	float width;
	float height;
};

struct LevelProp
{
	LevelProp(LevelPropOrientation orientation, const LevelPropDefinition &def, float x, float y)
		: orientation(orientation), collision_class(def.collision_class), collides_with_class(def.collides_with_class), x(x), y(y), width(def.get_width(orientation)), height(def.get_height(orientation)), excluder_padding_x(def.get_excluder_padding_x(orientation)), excluder_padding_y(def.get_excluder_padding_y(orientation))
	{
	}

	bool collide(const std::vector<LevelProp> &others) const
	{
		for (const LevelProp &other : others)
		{
			if (collide(other))
				return true;
		}

		return false;
	}

	bool collide(const LevelProp &other) const
	{
		return matches_collision_class(other.collision_class) &&
				x + width + excluder_padding_x > other.x - other.excluder_padding_x &&
				x - excluder_padding_x < other.x + other.width + other.excluder_padding_x &&
				y + height + excluder_padding_y > other.y - other.excluder_padding_y &&
				y - excluder_padding_y < other.y + other.height + other.excluder_padding_y;
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

	LevelPropOrientation orientation;
	int collision_class;
	int collides_with_class;
	float x, y, width, height;
	float excluder_padding_x, excluder_padding_y;
};

struct LevelWall
{
	static constexpr float HALFWIDTH = 0.05f;
	static constexpr float WIDTH = HALFWIDTH * 2.0f;

	LevelWall(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h) {}

	float x, y, w, h;
};

struct LevelFloorConnector
{
	LevelFloorConnector() = default;

	LevelFloorConnector(LevelSide side, float start, float stop)
		: side(side), start(start), stop(stop) {}

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

struct LevelFloor
{
	LevelFloor(Texture texture, float x, float y, float w, float h, bool skip_prop_generation = false)
		: texture(texture), skip_prop_generation(skip_prop_generation), x(x), y(y), w(w), h(h)  {}

	bool collide(const LevelFloor &rhs, float tolerance = 0.05f) const { return x + w > rhs.x + tolerance && x < (rhs.x + rhs.w) - tolerance && y + h > rhs.y + tolerance && y < (rhs.y + rhs.h) - tolerance; }
	bool collide(const std::vector<LevelFloor> &rhs) const
	{
		for (const auto &x : rhs)
			if (collide(x))
				return true;

		return false;
	}

	Texture texture;
	float x, y, w, h;
	bool skip_prop_generation;
	std::vector<LevelFloorConnector> connectors;
	std::vector<LevelProp> prop_spawns;
};
