#include "darktimes.h"

int main()
{
	win::system system;
	win::display display = system.make_display("Dark Times", win::display::screen_width(), win::display::screen_height(), win::display::FULLSCREEN);
	display.vsync(true);

	bool quit = false;
	display.event_button([&quit](const win::button button, bool press)
	{
		switch(button)
		{
			case win::button::ESC:
				quit = true;
				break;
		}
	});

	game::renderer renderer(display);
	game::world world;

	while(display.process() && !quit)
	{
		world.step();
		world.render(renderer);

		display.swap();
	}
}
