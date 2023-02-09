#pragma once

#include <win/Pool.hpp>

#include "../Entity.hpp"
#include "../component/Component.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/RenderableComponent.hpp"

void level_generate(
	int seed,
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &atlas_renderables,
	win::Pool<RenderableComponent> &tile_renderables
);
