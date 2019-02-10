#include "../darktimes.h"

game::renderer::renderer(win::display &display, win::roll &roll)
{
	screen.left = -8.0f;
	screen.right = 8.0f;
	screen.bottom = -4.5f;
	screen.top = 4.5f;

	font_renderer = display.make_font_renderer(display.width(), display.height(), screen.left, screen.right, screen.bottom, screen.top);
	font.title = font_renderer.make_font(roll["Nervous.ttf"], 0.5f);

	win::load_extensions();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
