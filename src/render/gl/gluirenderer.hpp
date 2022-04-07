#ifndef GL_UI_RENDERER_HPP
#define GL_UI_RENDERER_HPP

#include <chrono>

#include <win/assetroll.hpp>
#include <win/utility.hpp>
#include <win/fontrenderer.hpp>
#include <win/font.hpp>

#include "../../darktimes.hpp"

class GLUIRenderer
{
	NOCOPYMOVE(GLUIRenderer);

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

#endif
