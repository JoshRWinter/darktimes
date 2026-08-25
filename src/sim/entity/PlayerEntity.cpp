#include "../World.hpp"
#include "Entities.hpp"

void PlayerEntity::create(World &world)
{
    auto &ent = world.entities.add("player");
    ent.add(world.physicals.add(ent, -width / 2.0f, (-height / 2.0f), width, height, 0.0f));
    ent.add(world.players.add(ent));
    ent.add(world.renderables.add(ent, world.next_renderable(), Texture::player));
    ent.add(world.light_renderables.add(ent, world.next_renderable(), 0.0f, 0.0f, 4.5f, win::Color(0.8f, 0.8f, 0.3f), 0.0f, true));
}

void PlayerEntity::destroy(World &world, Entity &entity)
{
    world.physicals.remove(entity.remove<PhysicalComponent>());
    world.players.remove(entity.remove<PlayerComponent>());
    world.renderables.remove(entity.remove<RenderableComponent>());
    world.light_renderables.remove(entity.remove<LightRenderableComponent>());
    world.entities.remove(entity);
}
