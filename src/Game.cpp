#include "Game.hpp"
#include "render/Renderer.hpp"
#include "levelgen/LevelGenerator.hpp"

Game::Game(win::Display &display, win::AssetRoll &roll)
	: display(display)
	, roll(roll)
	, quit(false)
	, display_width(display.width())
	, display_height(display.height())
{}

void Game::play()
{
	Renderer renderer(win::Dimensions<int>(display.width(), display.height()), win::Area<float>(-8.0f, 8.0f, -4.5f, 4.5f), roll);

	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;
	std::vector<LevelProp> props;
	level_generate(69, floors, walls, props);
	sim.reset(floors, walls, props);

	std::vector<Renderable> renderables;
	for (const auto &f : floors)
		renderables.emplace_back(f.texture, f.x, f.y, f.w, f.h, 0.0f);
	for (const auto &w : walls)
		renderables.emplace_back(Texture::player, w.x, w.y, w.w, w.h, 0.0f);
	for (const auto &p : props)
		renderables.emplace_back(Texture::player, p.x, p.y, p.width, p.height, 0.0f);

	renderer.set_statics(renderables);
	renderer.set_view(0.0f, 0.0f, 0.5);
	renderer.set_view(0.0f, 0.0f, 0.0);

	RenderableWorldState *state = NULL;
	while (!quit)
	{
		display.process();

		// send the latest inputs
		sim.set_input(input);

		// get the latest world state from the simulation
		state = sim.get_state(state);

		// configure renderer for current frame
		if (state != NULL)
		{
			renderer.set_view(state->centerx, state->centery, 1.0f);
			renderer.set_dynamics(state->renderables);
		}

		renderer.render();

		// swap display front and back buffers
		display.swap();
	}
}

void Game::button_event(win::Button button, bool press)
{
	switch (button)
	{
		case win::Button::esc:
			quit = true;
			break;
		case win::Button::a:
		case win::Button::left:
			input.left = press;
			break;
		case win::Button::d:
		case win::Button::right:
			input.right = press;
			break;
		case win::Button::s:
		case win::Button::down:
			input.down = press;
			break;
		case win::Button::w:
		case win::Button::up:
			input.up = press;
			break;
		default: break;
	}
}

void Game::mouse_event(int x, int y)
{
	input.x = ((x / (float)display_width) * 16) - 8.0f;
	input.y = -(((y / (float)display_height) * 9) - 4.5f);
}

void Game::stop()
{
	quit = true;
}
