#pragma once

#include <win/pool.hpp>

#include "../entity.hpp"
#include "../component.hpp"

void level_generate(
	int seed,
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &atlas_renderables,
	win::Pool<RenderableComponent> &tile_renderables
);
