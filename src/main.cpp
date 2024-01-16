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
	display_options.width = 1600;
	display_options.height = 900;
#else
	display_options.caption = "Darktimes";
	display_options.fullscreen = true;
	display_options.width = win::Display::screen_width();
	display_options.height = win::Display::screen_height();
#endif
	display_options.gl_major = 3;
	display_options.gl_minor = 3;

	win::Display display(display_options);
	display.vsync(true);

	win::load_gl_functions();

	Game game(display, roll);

	display.register_button_handler([&game](win::Button button, bool press) { game.button_event(button, press); });
	display.register_window_handler([&game](win::WindowEvent event) { game.stop(); });

	game.play();

	return 0;
}
