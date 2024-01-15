#pragma once

#include <win/Display.hpp>
#include <win/AssetRoll.hpp>

#include "Darktimes.hpp"
#include "sim/Simulation.hpp"

class Game
{
	NO_COPY_MOVE(Game);

public:
	Game(win::Display &display, win::AssetRoll &roll);
	void play();
	void button_event(win::Button button, bool press);
	void mouse_event(float x, float y);
	void stop();

private:
	win::Display &display;
	win::AssetRoll &roll;
	Simulation sim;
	GameInput input;
	bool quit;
};
