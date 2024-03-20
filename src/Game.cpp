#include "Game.hpp"
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
	generate_level(renderer);

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

void Game::generate_level(Renderer &renderer)
{
	LevelGenerator generator;
	generator.generate(69);

	sim.reset(generator.floors, generator.walls, generator.props);

	std::vector<Renderable> renderables;
	for (const auto &f : generator.floors)
	{
		renderables.emplace_back(f.texture, f.x, f.y, f.w, f.h, 0.0f);
	}

	for (const auto &w : generator.walls)
	{
		renderables.emplace_back(Texture::player, w.x, w.y, w.w, w.h, 0.0f);
	}

	for (const auto &p : generator.props)
	{
		const auto newprop = correct_prop_orientation(p);
		renderables.emplace_back(p.texture, newprop.x, newprop.y, newprop.w, newprop.h, get_prop_rotation(newprop.side));
	}

	renderer.set_statics(renderables);
	renderer.set_view(0.0f, 0.0f, 0.0f);
}

LevelProp Game::correct_prop_orientation(const LevelProp &prop)
{
	LevelProp copy = prop;

	switch (prop.side)
	{
		case LevelSide::left:
		case LevelSide::right:
			break;
		case LevelSide::bottom:
		case LevelSide::top:
			copy.w = prop.h;
			copy.h = prop.w;
			break;
	}

	const float center_x = prop.x + (prop.w / 2.0f);
	const float center_y = prop.y + (prop.h / 2.0f);

	const float new_x = center_x - (copy.w / 2.0f);
	const float new_y = center_y - (copy.h / 2.0f);

	copy.x = new_x;
	copy.y = new_y;

	return copy;
}

float Game::get_prop_rotation(const LevelSide side)
{
	switch (side)
	{
		case LevelSide::left:
			return M_PI;
		case LevelSide::right:
			return 0.0f;
		case LevelSide::bottom:
			return (M_PI * 3.0f) / 2;
		case LevelSide::top:
			return M_PI / 2.0f;
	}

	win::bug("lolnope");
}
