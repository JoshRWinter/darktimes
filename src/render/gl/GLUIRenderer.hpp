#pragma once

#include <chrono>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>
#include <win/gl/GLTextRenderer.hpp>
#include <win/gl/GLFont.hpp>

#include "../../Darktimes.hpp"

class GLUIRenderer
{
	NO_COPY_MOVE(GLUIRenderer);

public:
	GLUIRenderer(const win::Dimensions<int>&, const win::Area<float>&, win::AssetRoll&);

	void set_seed(int);
	void draw();

private:
	win::Area<float> area;
	win::GLTextRenderer text_renderer;
	win::GLFont text_title;
	win::GLFont text_debug;

	char levelseed[20];
	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_sample;
	int accumulated_fps;
	char fps_str[20];
};
