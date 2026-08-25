#define _USE_MATH_DEFINES
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
    phys.rot = -controls.pan / 14.0f;

    // movement
    const float scoot = 0.045f;

    float dir = -1.0f;
    if (controls.up && controls.right && !controls.left && !controls.down)
        dir = M_PI / 4.0f;
    else if (controls.up && controls.left && !controls.right && !controls.down)
        dir = (M_PI + (M_PI / 2.0f)) / 2.0f;
    else if (controls.down && controls.right && !controls.up && !controls.left)
        dir = ((3.0f * M_PI / 2.0f) + M_PI * 2.0f) / 2.0f;
    else if (controls.down && controls.left && !controls.up && !controls.right)
        dir = (M_PI + (3 * M_PI / 2.0f)) / 2.0f;
    else if (controls.up && !controls.down && !controls.right && !controls.left)
        dir = M_PI / 2.0f;
    else if (controls.right && !controls.down && !controls.left && !controls.left)
        dir = 0.0f;
    else if (controls.left && !controls.down && !controls.right && !controls.up)
        dir = M_PI;
    else if (controls.down && !controls.up && !controls.right && !controls.left)
        dir = 3.0f * M_PI / 2.0f;

    if (dir != -1.0f)
    {
        phys.x += std::cosf(phys.rot + dir - M_PI / 2.0f) * scoot;
        phys.y += std::sinf(phys.rot + dir - M_PI / 2.0f) * scoot;
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
