#include "Game.hpp"
#include "render/Renderer.hpp"
#include "levelgen/LevelGenerator.hpp"

Game::Game(win::Display &display, win::AssetRoll &roll)
	: display(display)
	, roll(roll)
	, quit(false)
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

	renderer.load_statics(renderables);
	renderer.set_view(0.0f, 0.0f, 0.5);

	while (!quit)
	{
		display.process();

		renderer.render({});
		display.swap();
	}
}

void Game::button_event(win::Button button, bool press)
{
	if (button == win::Button::esc)
		quit = true;
}

void Game::mouse_event(float x, float y)
{
}

void Game::stop()
{
	quit = true;
}
