#include <cmath>

#include "../entity/Entities.hpp"
#include "Systems.hpp"

void player_system(World &world, const Controls &controls)
{
    if (world.players.size() == 0)
        PlayerEntity::create(world);

    auto &player = *world.players.begin();
    auto &phys = player.entity.get<PhysicalComponent>();

    // movement
    const float scoot = 0.065f;
    if (controls.up)
        phys.y += scoot;
    if (controls.down)
        phys.y -= scoot;
    if (controls.left)
        phys.x -= scoot;
    if (controls.right)
        phys.x += scoot;

    // aim direction
    const float aim = atan2f(controls.y, controls.x);
    phys.rot = aim;

    // collision
    for (const auto &p : world.index.level.query(win::SpatialIndexLocation(phys.x, phys.y, phys.w, phys.h)))
    {
        phys.correct(p);
    }
}
