#include <chrono>
#include <thread>

#include <win/Display.hpp>
#include <win/AssetRoll.hpp>

#include "render/gl/GLRenderer.hpp"
#include "render/gl/GLUIRenderer.hpp"
#include "sim/Simulation.hpp"

int main()
{
	// display setup
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

	// input handling
	bool quit = false;
	std::atomic<bool> simulation_quit = false;

	Input input;
	SyncObjectManager<Input> input_som;

	display.register_button_handler([&quit, &input, &input_som](win::Button button, bool press)
	{
		if (button == win::Button::esc)
			quit = true;
		else if (button == win::Button::up)
			input.up = press;
		else if (button == win::Button::down)
			input.down = press;
		else if (button == win::Button::left)
			input.left = press;
		else if (button == win::Button::right)
			input.right = press;
		else if (button == win::Button::enter)
			input.regenerate = press;

		// tell the sim
		Input *iso;
		do { iso = input_som.writer_acquire(); } while (iso == NULL);
		*iso = input;
		input_som.writer_release(iso);
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

	// renderer setup
	win::AssetRoll roll("Darktimes.roll");
	GLRenderer renderer(win::Area<float>(-8.0f, 8.0f, -4.5f, 4.5f), roll);
	GLUIRenderer uirenderer(win::Dimensions<int>(display.width(), display.height()), win::Area<float>(-8.0f, 8.0f, -4.5f, 4.5f), roll);

	// set up the simulation thread
	SyncObjectManager<LevelRenderState> level_render_state_som;
	SyncObjectManager<RenderState> render_state_som;
	std::thread simulation_thread(simulation, std::ref(simulation_quit), std::ref(level_render_state_som), std::ref(render_state_som), std::ref(input_som));

	renderer.set_center(0.0f, 0.0f);
	// loop de loop
	while(!quit)
	{
		display.process();

		RenderState *rs;
		if ((rs = render_state_som.reader_acquire()) != NULL)
		{
			renderer.set_center(rs->centerx, rs->centery);
			render_state_som.reader_release(rs);
		}

		LevelRenderState *const lrsso = level_render_state_som.reader_acquire();
		if (lrsso != NULL)
		{
			renderer.set_level_data(lrsso->tile_renderables, lrsso->atlas_renderables);
			uirenderer.set_seed(lrsso->seed);

			level_render_state_som.reader_release(lrsso);
		}

		renderer.draw();
		uirenderer.draw_gamehud();
		display.swap();
	}

	simulation_quit = true;
	simulation_thread.join();

	return 0;
}
