#pragma once

#include <vector>
#include <cmath>

#include "LevelObjectsInternal.hpp"

struct LevelPropDefinition
{
	static constexpr float default_excluder_padding = 0.2f;

	// bitflags
	static constexpr int collision_class_none = 0;
	static constexpr int collision_class_excluder = 1;
	static constexpr int collision_class_furniture = 2;
	static constexpr int collision_class_rug = 4;
	static constexpr int collision_class_debris = 8;

	LevelPropDefinition(Texture texture, int collision_class, int collides_with_class, float width, float height, float excluder_padding_x = default_excluder_padding, float excluder_padding_y = default_excluder_padding)
		: texture(texture)
		, collision_class(collision_class)
		, collides_with_class(collides_with_class)
		, excluder_padding_x(excluder_padding_x)
		, excluder_padding_y(excluder_padding_y)
		, width(width)
		, height(height)
	{}

	LevelPropInternal spawn(LevelSide side, float x, float y) const
	{
		float w, h;
		switch (side)
		{
			case LevelSide::left:
			case LevelSide::right:
				w = width;
				h = height;
				break;
			case LevelSide::bottom:
			case LevelSide::top:
				w = height;
				h = width;
				break;
		}

		return LevelPropInternal(texture, side, collision_class, collides_with_class, x, y, w, h, excluder_padding_x, excluder_padding_y);
	}

	float get_width(LevelSide side) const
	{
		return side == LevelSide::left || side == LevelSide::right ? width : height;
	}

	float get_height(LevelSide side) const
	{
		return side == LevelSide::left || side == LevelSide::right ? height : width;
	}

private:
	Texture texture;
	int collision_class;
	int collides_with_class;
	float excluder_padding_x;
	float excluder_padding_y;
	float width;
	float height;
};

struct PropDefinitions
{
	static const PropDefinitions &get() { static PropDefinitions pd; return pd; }

	std::vector<LevelPropDefinition> side_tables =
	{
		LevelPropDefinition(Texture::large_table, LevelPropDefinition::collision_class_furniture, LevelPropDefinition::collision_class_excluder | LevelPropDefinition::collision_class_furniture, 0.36f, 0.7f)
	};

	std::vector<LevelPropDefinition> center_tables =
	{
		LevelPropDefinition(Texture::large_table, LevelPropDefinition::collision_class_furniture, LevelPropDefinition::collision_class_excluder | LevelPropDefinition::collision_class_furniture, 0.5f, 0.5f, 0.1f, 0.1f)
	};

	std::vector<LevelPropDefinition> rugs =
	{
		LevelPropDefinition(Texture::large_table, LevelPropDefinition::collision_class_rug, LevelPropDefinition::collision_class_rug, 1.0f, 2.0f, 0.0f, 0.0f)
	};

	LevelPropDefinition floor_transition_strip = LevelPropDefinition(Texture::player, LevelPropDefinition::collision_class_debris, 0.0f, 0.025f, 1.0f, 0.0f, 0.0f);

private:
	PropDefinitions() = default;
};
