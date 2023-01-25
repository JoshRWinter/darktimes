#pragma once

#include <win/pool.hpp>

#include "entity.hpp"
#include "component.hpp"

struct World
{
	win::Pool<Entity> entities;
	win::Pool<PhysicalComponent> physicals;
	win::Pool<RenderableComponent> atlas_renderables;

	void reset()
	{
		entities.clear();
		physicals.clear();
		atlas_renderables.clear();
	}
};
