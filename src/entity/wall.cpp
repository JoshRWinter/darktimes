#include "../darktimes.h"

ent::wall::wall(float xp, float yp, float width, float height)
{
	x = xp;
	y = yp;
	w = width;
	h = height;
	rot = 0.0f;
}

std::vector<ent::wall> ent::wall::generate()
{
	std::vector<ent::wall> walls;

	walls.push_back({0.0f, -4.0f, ent::wall::THICKNESS, 5.0f});

	return walls;
}

void ent::wall::render(game::renderer &renderer, const std::vector<ent::wall> &walls)
{
	for(const ent::wall &wall : walls)
	{
		renderer.quadpass.add(wall);
	}
}
