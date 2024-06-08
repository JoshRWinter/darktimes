#pragma once

#include <vector>
#include <cmath>

#include "LevelObjectsInternal.hpp"

struct LevelPropDefinition
{
	static constexpr float default_excluder_padding = 0.2f;

	LevelPropDefinition(Texture texture, bool solid, float width, float height, float excluder_padding_x = default_excluder_padding, float excluder_padding_y = default_excluder_padding)
		: texture(texture)
		, solid(solid)
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

		return LevelPropInternal(texture, side, solid, x, y, w, h, excluder_padding_x, excluder_padding_y);
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
	bool solid;
	float excluder_padding_x;
	float excluder_padding_y;
	float width;
	float height;
};

struct PropDefinitions
{
	static const PropDefinitions &get() { static PropDefinitions pd; return pd; }

	std::vector<LevelPropDefinition> side_furniture =
	{
		LevelPropDefinition(Texture::chair1, true, 0.4f, 0.6f),
		LevelPropDefinition(Texture::chair2, true, 0.4f, 0.6f),
		LevelPropDefinition(Texture::side_table1, true, 0.4333f, 1.0f)
	};

	std::vector<LevelPropDefinition> center_tables =
	{
		LevelPropDefinition(Texture::large_table, true, 0.8f, 1.5f, 0.1f, 0.1f),
		LevelPropDefinition(Texture::large_table2, true, 0.8f, 1.25f, 0.1f, 0.1f)
	};

	std::vector<LevelPropDefinition> rugs =
	{
		LevelPropDefinition(Texture::rug1, false, 2.0f, 1.0f, 0.0f, 0.0f),
		LevelPropDefinition(Texture::rug2, false, 2.0f, 1.325f, 0.0f, 0.0f)
	};

	LevelPropDefinition floor_transition_strip = LevelPropDefinition(Texture::transition_strip, false, 0.025f, 1.0f, 0.0f, 0.0f);

private:
	PropDefinitions() = default;
};
