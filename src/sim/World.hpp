#pragma once

#include <vector>

#include <win/Pool.hpp>
#include <win/SpatialIndex.hpp>

#include "component/Components.hpp"
#include "entity/Entities.hpp"
#include "levelgen/LevelObjects.hpp"

struct World
{
    template<typename T> using Pool = win::Pool<T, 50, false>;

    // gameplay state

    Pool<Entity> entities;
    Pool<PhysicalComponent> physicals;
    Pool<RenderableComponent> renderables;
    Pool<PlayerComponent> players;

    win::SpatialIndex<PhysicalComponent> index;
};
