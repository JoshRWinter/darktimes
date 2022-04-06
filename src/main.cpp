#include <chrono>
#include <thread>

#include <win/display.hpp>
#include <win/assetroll.hpp>

#include "render/gl/glrenderer.hpp"
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
		if (button == win::Button::esc)
			quit = true;

		else if (button == win::Button::up)
			up = press;
		else if (button == win::Button::down)
			down = press;
		else if (button == win::Button::left)
			left = press;
		else if (button == win::Button::right)
			right = press;
	});

	display.register_window_handler([&quit](win::WindowEvent event)
	{
		switch (event)
		{
		case win::WindowEvent::close:
			quit = true;
			break;
		default: break;
		}
	});

	win::AssetRoll roll("darktimes.bin");
	GLRenderer renderer(win::Dimensions<int>(display.width(), display.height()), win::Area<float>(-8.0f, 8.0f, -4.5f, 4.5f), win::Area<float>(-8.0f, 8.0f, -4.5f, 4.5f), roll);

	LargeSyncObject<STR_LevelDataSyncObject> str_level_data_sync;
	std::thread simulation_thread(simulation, std::ref(simulation_quit), std::ref(str_level_data_sync));

	float x = 0.0f, y = 0.0f;
	while(!quit)
	{
		display.process();

		const float scoot = 0.1f;
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

		renderer.send_frame();
		display.swap();
	}

	simulation_quit = true;
	simulation_thread.join();

	return 0;
}
