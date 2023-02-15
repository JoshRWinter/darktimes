#pragma once

enum class Texture
{
	// floors
	level_floor_start = 100,
	level_floor_1 = 100,
	level_floor_2,
	level_floor_end = level_floor_2,

	// props
	level_prop_start = 200,
	level_prop_sidetable = 200,
	level_prop_end = level_prop_sidetable,

	// gameplay
	gameplay_start = 300,
	player = 300
};
