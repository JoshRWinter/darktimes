#include <ratio>

#include "GLUIRenderer.hpp"

GLUIRenderer::GLUIRenderer(const win::Dimensions<int> &dims, const win::Area<float> &area, win::AssetRoll &roll)
	: area(area)
	, text_renderer(dims, area)
	, text_title(dims, area, 0.7f, roll["font/CHE-THIS.TTF"])
	, text_debug(dims, area, 0.25f, roll["font/NotoSansMono-Regular.ttf"])
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

void GLUIRenderer::draw()
{
	++accumulated_fps;
	auto now = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration<float, std::milli>(now - last_fps_sample).count() > 1000)
	{
		snprintf(fps_str, sizeof(fps_str), "%d", accumulated_fps);
		last_fps_sample = std::chrono::high_resolution_clock::now();
		accumulated_fps = 0;
	}

	text_renderer.draw(text_title, "DARK TIMES", 0.0f, 3.75f, win::Color(1.0f, 1.0f, 1.0f), true);
	text_renderer.draw(text_debug, levelseed, 0.0f, -4.25f, win::Color(1.0f, 1.0f, 0.0f), true);
	text_renderer.draw(text_debug, fps_str, area.left + 0.1f, area.top - text_debug.font_metric().size, win::Color(1.0f, 1.0f, 0.0f));

	text_renderer.flush();
}
