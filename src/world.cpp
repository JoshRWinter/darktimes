#include "darktimes.h"

game::world::world()
{
}

void game::world::step()
{
	if(entity.walls.size() == 0)
		entity.walls = ent::wall::generate();
}

void game::world::render(game::renderer &renderer)
{
	glClear(GL_COLOR_BUFFER_BIT);

	renderer.font_renderer.draw(renderer.font.title, "Dark Times", 0.0f, 3.0f, win::color(1.0f, 1.0f, 1.0f), win::font_renderer::CENTERED);

	// draw walls
	ent::wall::render(renderer, entity.walls);

	renderer.quadpass.send();
}
