#include <GL/gl.h>

#include <win.h>
#include "Renderer.hpp"

Renderer::Renderer(int iwidth, int iheight, float left, float right, float bottom, float top, win::AssetRoll &roll)
	: font_renderer(iwidth, iheight, left, right, bottom, top)
	, font_debug(font_renderer, roll["font/NotoSansMono-Regular.ttf"], 0.25f)
	, font_ui(font_renderer, roll["font/CHE-THIS.TTF"], 0.7f)
	, last_fps_calc_time(std::chrono::high_resolution_clock::now())
	, accumulated_fps(0)
	, left(left)
	, right(right)
	, bottom(bottom)
	, top(top)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	strcpy(fpsindicator, "0");
}

void Renderer::computeframe()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - last_fps_calc_time).count() > 1000)
	{
		snprintf(fpsindicator, sizeof(fpsindicator), "%d", accumulated_fps);
		accumulated_fps = 0;
		last_fps_calc_time = std::chrono::high_resolution_clock::now();
	}
	else ++accumulated_fps;

	font_renderer.draw(font_debug, fpsindicator, left + 0.05f, top - font_debug.size(), win::Color(1.0f, 1.0f, 0.0f, 1.0f));
	font_renderer.draw(font_ui, "Dark Times\nPlay\nSettings\nAbout\nQuit", 0.0f, 2.0f, win::Color(1.0f, 1.0f, 1.0f, 1.0f), true);
}
