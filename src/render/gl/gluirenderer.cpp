#include <ratio>

#include <string.h>

#include "gluirenderer.hpp"

GLUIRenderer::GLUIRenderer(const win::Dimensions<int> &dims, const win::Area<float> &area, win::AssetRoll &roll)
	: area(area)
	, font_renderer(dims, area)
	, font_title(font_renderer, roll["font/CHE-THIS.TTF"], 0.7f)
	, font_debug(font_renderer, roll["font/NotoSansMono-Regular.ttf"], 0.25f)
{
	levelseed[0] = 0;
	accumulated_fps = 0;
	fps_str[0] = '0'; fps_str[1] = 0;
	last_fps_sample = std::chrono::high_resolution_clock::now();
}

void GLUIRenderer::set_seed(int seed)
{
	snprintf(levelseed, sizeof(levelseed), "%d", seed);
}

void GLUIRenderer::draw_gamehud()
{
	++accumulated_fps;
	auto now = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration<float, std::milli>(now - last_fps_sample).count() > 1000)
	{
		snprintf(fps_str, sizeof(fps_str), "%d", accumulated_fps);
		last_fps_sample = std::chrono::high_resolution_clock::now();
		accumulated_fps = 0;
	}

	font_renderer.draw(font_title, "DARK TIMES", 0.0f, 3.75f, win::Color(1.0f, 1.0f, 1.0f), true);
	font_renderer.draw(font_debug, levelseed, 0.0f, -4.25f, win::Color(1.0f, 1.0f, 0.0f), true);
	font_renderer.draw(font_debug, fps_str, area.left + 0.1f, area.top - font_debug.size() , win::Color(1.0f, 1.0f, 0.0f));
}
