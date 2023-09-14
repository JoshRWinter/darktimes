#include <chrono>
#include <thread>

#include <win/Display.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "render/Renderer.hpp"
#include "sim/Simulation.hpp"

static void button_event(const win::Button button, const bool press, Input &input)
{
	switch (button)
	{
		case win::Button::left:
			input.left = press;
			break;
		case win::Button::right:
			input.right = press;
			break;
		case win::Button::down:
			input.down = press;
			break;
		case win::Button::up:
			input.up = press;
			break;

		default: break;
	}
}

int main()
{
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

	win::load_gl_functions();

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

		button_event(button, press, input);
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

	win::AssetRoll roll("Darktimes.roll");

	Renderer renderer(win::Dimensions<int>(display.width(), display.height()), win::Area<float>(-8.0f, 8.0f, -4.5f, 4.5f), roll);

	// set up the simulation thread
	SyncObjectManager<LevelRenderState> level_render_state_som;
	SyncObjectManager<RenderState> render_state_som;
	std::thread simulation_thread(simulation, std::ref(simulation_quit), std::ref(level_render_state_som), std::ref(render_state_som), std::ref(input_som));

	// loop de loop
	while(!quit)
	{
		display.process();

		// publish the latest inputs
		Input *i;
		if ((i = input_som.writer_acquire()) != NULL)
		{
			*i = input;
			input_som.writer_release(i);
		}

		// gather the latest info about render state
		RenderState *rs;
		if ((rs = render_state_som.reader_acquire()) != NULL)
		{
			renderer.set_view(rs->centerx, rs->centery, 0.4f);
			render_state_som.reader_release(rs);
		}

		// see if the sim has published a new level layout
		LevelRenderState *const lrsso = level_render_state_som.reader_acquire();
		if (lrsso != NULL)
		{
			renderer.set_level_objects(lrsso->renderables);

			level_render_state_som.reader_release(lrsso);
		}

		renderer.render();

		display.swap();
	}

	simulation_quit = true;
	simulation_thread.join();

	return 0;
}
