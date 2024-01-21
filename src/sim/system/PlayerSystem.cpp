#include <cmath>

#include "PlayerSystem.hpp"

#include "../entity/PlayerEntity.hpp"

void player_system(
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &renderables,
	win::Pool<PlayerComponent> &players,
	const GameInput &input
)
{
	if (players.size() == 0)
		PlayerEntity::create(
			entities,
			physicals,
			renderables,
			players
		);

	auto &player = *players.begin();
	auto &phys = player.entity.get<PhysicalComponent>();

	// movement
	const float scoot = 0.1f;
	if (input.up)
		phys.y += scoot;
	if (input.down)
		phys.y -= scoot;
	if (input.left)
		phys.x -= scoot;
	if (input.right)
		phys.x += scoot;

	// aim direction
	const float aim = atan2f(input.y, input.x);
	phys.rot = aim;

	// collision
	for (const auto &p : physicals)
	{
		if (&p == &phys)
			continue;

		phys.correct(p);
	}
}
