#include <cmath>

#include "../entity/Entities.hpp"
#include "Systems.hpp"

void player_system(World &world, const Controls &controls)
{
    if (world.players.size() == 0)
        PlayerEntity::create(world);

    auto &player = *world.players.begin();
    auto &phys = player.entity.get<PhysicalComponent>();
    auto &light = player.entity.get<LightRenderableComponent>();

    // aim direction
    phys.rot = atan2f(controls.y, controls.x);

    // movement
    const float scoot = 0.045f;
    if (controls.up)
    {
        phys.x += std::cosf(phys.rot) * scoot;
        phys.y += std::sinf(phys.rot) * scoot;
    }
    if (controls.down)
    {
        phys.x -= std::cosf(phys.rot) * scoot;
        phys.y -= std::sinf(phys.rot) * scoot;
    }
    if (controls.left)
    {
        phys.x += std::cosf(phys.rot + M_PI / 2.0f) * scoot;
        phys.y += std::sinf(phys.rot + M_PI / 2.0f) * scoot;
    }
    if (controls.right)
    {
        phys.x += std::cosf(phys.rot - M_PI / 2.0f) * scoot;
        phys.y += std::sinf(phys.rot - M_PI / 2.0f) * scoot;
    }

    // collision
    for (const auto &p : world.index.level.query(win::SpatialIndexLocation(phys.x, phys.y, phys.w, phys.h)))
    {
        phys.correct(p);
    }

    light.x = phys.x + phys.w / 2.0f + std::cos(phys.rot - 0.5f) * 0.25f;
    light.y = phys.y + phys.h / 2.0f + std::sin(phys.rot - 0.5f) * 0.25f;
    light.angle = phys.rot;
}
