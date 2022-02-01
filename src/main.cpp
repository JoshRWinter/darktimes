#include <win.h>
#include <chrono>

#include "assetmanager.hpp"
#include "render/renderer.hpp"
#include "sim/simulation.hpp"

int main()
{
	win::DisplayOptions display_options;
#ifndef NDEBUG
	display_options.caption = "debug_window";
	display_options.fullscreen = 0;
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

	bool quit = false;
	std::atomic<bool> simulation_quit = false;
	bool up = false, down = false, left = false, right = false;
	display.register_button_handler([&quit, &up, &down, &left, &right](win::Button button, bool press)
	{
		if (button == win::Button::ESC)
			quit = true;

		else if (button == win::Button::UP)
			up = press;
		else if (button == win::Button::DOWN)
			down = press;
		else if (button == win::Button::LEFT)
			left = press;
		else if (button == win::Button::RIGHT)
			right = press;
	});

	display.register_window_handler([&quit](win::WindowEvent event)
	{
		switch (event)
		{
		case win::WindowEvent::CLOSE:
			quit = true;
			break;
		default: break;
		}
	});

	win::AssetRoll roll("darktimes.bin");
	AssetManager assetmanager(roll);
	Renderer renderer(display.width(), display.height(), -16.0f, 16.0f, -2.0f, 18.0f, assetmanager);
	// Renderer renderer(display.width(), display.height(), -32.0f, 32.0f, -18.0f, 18.0f, assetmanager);
	// Renderer renderer(display.width(), display.height(), -16.0f, 16.0f, -9.0f, 9.0f, assetmanager);

	LargeSyncObject<STR_LevelDataSyncObject> str_level_data_sync;
	std::thread simulation_thread(simulation, std::ref(simulation_quit), std::ref(str_level_data_sync));

	float x = 0.0f, y = 0.0f;
	while(!quit)
	{
		display.process();

		const float scoot = 0.4f;
		if (up) y += scoot;
		if (down) y -= scoot;
		if (left) x -= scoot;
		if (right) x += scoot;
		renderer.set_center(x, y);
		if (str_level_data_sync.dirty())
		{
			std::unique_ptr<STR_LevelDataSyncObject> leveldata;
			str_level_data_sync.get(leveldata);
			renderer.set_level_data(leveldata->floors, leveldata->walls, leveldata->props, leveldata->seed);
		}

		renderer.computeframe();
		display.swap();
	}

	simulation_quit = true;
	simulation_thread.join();

	return 0;
}
