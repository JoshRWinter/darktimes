#include <win/Display.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "Game.hpp"

int main()
{
	win::AssetRoll roll("Darktimes.roll");

	// display setup
	win::DisplayOptions display_options;
#ifndef NDEBUG
	display_options.caption = "debug_window";
	display_options.fullscreen = false;
	display_options.width = 1920;
	display_options.height = 1080;
	display_options.debug = false;
#else
	display_options.caption = "Darktimes";
	display_options.fullscreen = true;
	display_options.width = 1;
	display_options.height = 1;
#endif
	display_options.gl_major = 3;
	display_options.gl_minor = 3;

	win::Display display(display_options);
	display.vsync(true);

	win::load_gl_functions();

	Game game(display, roll);

	display.register_mouse_handler([&game](int x, int y) { game.mouse_event(x, y); });
	display.register_button_handler([&game](win::Button button, bool press) { game.button_event(button, press); });
	display.register_window_handler([&game](win::WindowEvent event) { game.stop(); });

	game.play();

	return 0;
}
