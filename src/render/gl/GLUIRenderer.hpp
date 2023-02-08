#pragma once

#include <chrono>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>
#include <win/FontRenderer.hpp>
#include <win/Font.hpp>

#include "../../Darktimes.hpp"

class GLUIRenderer
{
	NO_COPY_MOVE(GLUIRenderer);

public:
	GLUIRenderer(const win::Dimensions<int>&, const win::Area<float>&, win::AssetRoll&);

	void set_seed(int);
	void draw_gamehud();

private:
	win::Area<float> area;
	win::FontRenderer font_renderer;
	win::Font font_title;
	win::Font font_debug;

	char levelseed[20];
	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_sample;
	int accumulated_fps;
	char fps_str[20];
};
