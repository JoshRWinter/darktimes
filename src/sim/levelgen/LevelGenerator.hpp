#pragma once

#include <win/Pool.hpp>

#include "../Entity.hpp"
#include "../Component.hpp"

void level_generate(
	int seed,
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &atlas_renderables,
	win::Pool<RenderableComponent> &tile_renderables
);
