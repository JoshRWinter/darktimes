#pragma once

#include <win/Display.hpp>
#include <win/AssetRoll.hpp>

#include "Darktimes.hpp"
#include "render/Renderer.hpp"
#include "sim/Simulation.hpp"

class Game
{
	NO_COPY_MOVE(Game);

public:
	Game(win::Display &display, win::AssetRoll &roll);
	void play();
	void button_event(win::Button button, bool press);
	void mouse_event(int x, int y);
	void stop();

private:
	void generate_level(Renderer &renderer);
	static LevelProp correct_prop_orientation(const LevelProp &prop);
	static float get_prop_rotation(const LevelSide side);

	win::Display &display;
	win::AssetRoll &roll;
	Simulation sim;
	GameInput input;
	bool quit;
	int display_width;
	int display_height;
};
