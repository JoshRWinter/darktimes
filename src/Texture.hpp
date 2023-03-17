#pragma once

enum class Texture : int
{
	// floors
	level_floor_start = 100,
	level_floor_1 = 100,
	level_floor_2,
	level_floor_end,

	// props
	level_prop_start = 200,
	level_prop_sidetable = 200,
	level_prop_end,

	// gameplay
	gameplay_start = 300,
	player = 300,
	misc,
	gameplay_end
};
