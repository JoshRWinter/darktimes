#include "../darktimes.h"

game::renderer::renderer(win::display &display, win::roll &roll)
	: screen(-8.0f, 8.0f, -4.5f, 4.5)
	, quadpass(roll, screen)
{
	font_renderer = display.make_font_renderer(display.width(), display.height(), screen.left, screen.right, screen.bottom, screen.top);
	font.title = font_renderer.make_font(roll["Nervous.ttf"], 0.5f);

	win::load_extensions();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
