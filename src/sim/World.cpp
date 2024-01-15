#include "World.hpp"

void World::set_input(const GameInput &i)
{
	input = i;
}

RenderableWorldState World::get_state()
{
	RenderableWorldState result;

	result.centerx = centerx;
	result.centery = centery;

	return result;
}

void World::tick()
{
	const float scoot = 0.3f;
	if (input.left)
		centerx -= scoot;
	if (input.right)
		centerx += scoot;
	if (input.down)
		centery -= scoot;
	if (input.up)
		centery += scoot;
}
