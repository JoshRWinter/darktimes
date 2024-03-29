#include "../../Texture.hpp"
#include "PlayerEntity.hpp"

void PlayerEntity::create(
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &renderables,
	win::Pool<PlayerComponent> &players
)
{
	auto &ent = entities.add("player");
	ent.add(physicals.add(ent, -width / 2.0f, (-height / 2.0f) - 0.8f, width, height, 0.0f));
	ent.add(players.add(ent));
	ent.add(renderables.add(ent, Texture::player));
}
