#include <cmath>

#include "../entity/Entities.hpp"
#include "Systems.hpp"

void player_system(World &world, const GameInput &input)
{
    if (world.players.size() == 0)
        PlayerEntity::create(world);

    auto &player = *world.players.begin();
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
    for (const auto &p : world.index.query(win::SpatialIndexLocation(phys.x, phys.y, phys.w, phys.h)))
    {
        phys.correct(p);
    }
}
