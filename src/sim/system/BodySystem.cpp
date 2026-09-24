#define _USE_MATH_DEFINES
#include <cmath>

#include "Systems.hpp"

void body_system(World &world)
{
    for (auto &body : world.bodies)
    {
        const auto &phys = body.entity.components.get<PhysicalComponent>();
        const float centerx = phys.x + phys.w / 2.0f;
        const float centery = phys.y + phys.h / 2.0f;

        body.torso.from_physical(phys);

        const float extent = 0.04f;
        const float speed = 7.0f;
        const float spread = 0.08;

        const float rest_left_x = centerx + std::cosf(phys.rot - M_PI / 2.0f) * spread - body.left.w / 2.0f;
        const float rest_left_y = centery + std::sinf(phys.rot - M_PI / 2.0f) * spread - body.left.h / 2.0f;
        body.left.x = rest_left_x + std::cosf(phys.rot) * (std::sinf(body.cycle / speed) * extent);
        body.left.y = rest_left_y + std::sinf(phys.rot) * (std::sinf(body.cycle / speed) * extent);
        body.left.rot = phys.rot;

        const float rest_right_x = centerx + std::cosf(phys.rot + M_PI / 2.0f) * spread - body.right.w / 2.0f;
        const float rest_right_y = centery + std::sinf(phys.rot + M_PI / 2.0f) * spread - body.right.h / 2.0f;
        body.right.x = rest_right_x - std::cosf(phys.rot) * (std::sinf(body.cycle / speed) * extent);
        body.right.y = rest_right_y - std::sinf(phys.rot) * (std::sinf(body.cycle / speed) * extent);
        body.right.rot = phys.rot;

        const float rest_head_x = centerx - body.head.w / 2.0f;
        const float rest_head_y = centery - body.head.h / 2.0f;
        const float headbob_speed = 7.0f;
        const float headbob_extent = 0.01f;
        body.head.x = rest_head_x + std::cosf(phys.rot) * (std::sinf(body.cycle / headbob_speed) * headbob_extent);
        body.head.y = rest_head_y + std::sinf(phys.rot) * (std::sinf(body.cycle / headbob_speed) * headbob_extent);
        body.head.rot = phys.rot;

        body.torso.rot += std::sinf(body.cycle / speed) * 0.1f;

        if (body.moving)
            ++body.cycle;
    }
}
