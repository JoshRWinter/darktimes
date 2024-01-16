#include "World.hpp"

RenderableWorldState World::tick(const GameInput &input)
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

	RenderableWorldState state;
	state.centerx = centerx;
	state.centery = centery;
	return state;
}
