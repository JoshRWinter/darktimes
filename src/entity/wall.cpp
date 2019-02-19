#include "../darktimes.h"

ent::wall::wall(float xp, float yp, float width, float height)
{
	x = xp;
	y = yp;
	w = width;
	h = height;
	rot = 0.0f;
}

void ent::wall::render(game::renderer &renderer, const std::vector<ent::wall> &walls)
{
	for(const ent::wall &wall : walls)
	{
		renderer.quadpass.add(wall);
	}
}
