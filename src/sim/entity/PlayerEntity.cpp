#include "../World.hpp"
#include "Entities.hpp"

void PlayerEntity::create(World &world)
{
    auto &ent = world.entities.add("player");
    ent.add(world.physicals.add(ent, -width / 2.0f, (-height / 2.0f) - 0.8f, width, height, 0.0f));
    ent.add(world.players.add(ent));
    ent.add(world.renderables.add(ent, Texture::player));
}

void PlayerEntity::destroy(World &world, Entity &entity)
{
    world.physicals.remove(entity.remove<PhysicalComponent>());
    world.players.remove(entity.remove<PlayerComponent>());
    world.renderables.remove(entity.remove<RenderableComponent>());
    world.entities.remove(entity);
}
