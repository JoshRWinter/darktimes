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
    Pool<PhysicalComponent> physicals;
    Pool<RenderableComponent> renderables;
    Pool<LightRenderableComponent> light_renderables;
    Pool<PlayerComponent> players;

    struct
    {
        win::SpatialIndex<PhysicalComponent> level;
    } index;

    int next_renderable() { return ++renderableid; }

    void reset()
    {
        entities.clear();
        physicals.clear();
        renderables.clear();
        light_renderables.clear();
        players.clear();
    }

private:
    int renderableid = 0;
};
