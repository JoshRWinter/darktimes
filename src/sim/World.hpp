#pragma once

#include <win/Pool.hpp>
#include <win/SpatialIndex.hpp>

#include "component/Components.hpp"
#include "entity/Entities.hpp"

struct World
{
    template<typename T> using Pool = win::Pool<T, 50, false>;

    // gameplay state

    Pool<Entity> entities;
    Entity::ComponentBag component_bag;

    Pool<PhysicalComponent> physicals;
    Pool<RenderableComponent> renderables;
    Pool<LightRenderableComponent> light_renderables;
    Pool<PlayerComponent> players;
    Pool<BodyComponent> bodies;

    struct
    {
        win::SpatialIndex<PhysicalComponent> level;
    } index;

    int next_renderable() { return ++renderableid; }

private:
    int renderableid = 0;
};
