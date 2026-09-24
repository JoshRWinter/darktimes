#include "../World.hpp"
#include "Entities.hpp"

void PlayerEntity::create(World &world)
{
    auto &ent = world.entities.add(world.component_bag, "player");
    ent.components.add(world.physicals.add(ent, -width / 2.0f, (-height / 2.0f), width, height, 0.0f));
    ent.components.add(world.players.add(ent));
    ent.components.add(world.light_renderables.add(ent, world.next_renderable(), 0.0f, 0.0f, 4.5f, win::Color(0.8f, 0.8f, 0.3f), 0.0f, true));
    auto &left = ent.components.add(world.renderables.add(ent, world.next_renderable(), leg_width, leg_height, Texture::player_leg));
    auto &right = ent.components.add(world.renderables.add(ent, world.next_renderable(), leg_width, leg_height, Texture::player_leg));
    auto &torso = ent.components.add(world.renderables.add(ent, world.next_renderable(), torso_width, torso_height, Texture::player_torso));
    auto &head = ent.components.add(world.renderables.add(ent, world.next_renderable(), head_width, head_height, Texture::player_head));
    ent.components.add(world.bodies.add(ent, head, torso, left, right));
}

void PlayerEntity::destroy(World &world, Entity &entity)
{
    world.physicals.remove(entity.components.remove<PhysicalComponent>());
    world.players.remove(entity.components.remove<PlayerComponent>());
    world.renderables.remove(entity.components.remove<RenderableComponent>());
    world.renderables.remove(entity.components.remove<RenderableComponent>());
    world.renderables.remove(entity.components.remove<RenderableComponent>());
    world.light_renderables.remove(entity.components.remove<LightRenderableComponent>());
    world.entities.remove(entity);
}
