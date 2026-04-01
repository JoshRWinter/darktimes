#pragma once

#include <vector>

#include <win/Pool.hpp>
#include <win/SpatialIndex.hpp>

#include "component/Component.hpp"
#include "component/PhysicalComponent.hpp"
#include "component/PlayerComponent.hpp"
#include "component/RenderableComponent.hpp"
#include "entity/Entity.hpp"
#include "levelgen/LevelObjects.hpp"
#include "PoolTypes.hpp"

struct World
{
    World() = default;

    // Level gen data
    std::vector<LevelFloor> level_floors;
    std::vector<LevelProp> level_props;
    std::vector<LevelWall> level_walls;

    // gameplay state
    win::SpatialIndex<PhysicalComponent> index;
    Pool<Entity> entities;
    Pool<PhysicalComponent> physicals;
    Pool<RenderableComponent> renderables;
    Pool<PlayerComponent> players;
};
