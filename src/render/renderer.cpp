#include "../darktimes.h"

game::renderer::renderer(win::display &display)
{
	screen.left = -8.0f;
	screen.right = 8.0f;
	screen.bottom = -4.5f;
	screen.top = 4.5f;

	win::load_extensions();
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}
