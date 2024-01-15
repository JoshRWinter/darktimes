#include "PlayerSystem.hpp"

#include "../entity/PlayerEntity.hpp"

void player_system(
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &renderables,
	win::Pool<PlayerComponent> &players,
	float &centerx,
	float &centery
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
	auto &phys = player.entity.rget<PhysicalComponent>();

	/*
	const float scoot = 0.4f;
	if (input.up)
		phys.y += scoot;
	if (input.down)
		phys.y -= scoot;
	if (input.left)
		phys.x -= scoot;
	if (input.right)
		phys.x += scoot;
	 */

	// center the screen on the player
	centerx = phys.x + (PlayerEntity::width / 2.0f);
	centery = phys.y + (PlayerEntity::height / 2.0f);
}
