#pragma once

#include <win/Pool.hpp>

#include "Entity.hpp"
#include "component/Component.hpp"
#include "component/PhysicalComponent.hpp"
#include "component/RenderableComponent.hpp"

struct World
{
	~World() { reset(); }

	win::Pool<Entity> entities;
	win::Pool<PhysicalComponent> physicals;
	win::Pool<RenderableComponent> atlas_renderables;
	win::Pool<RenderableComponent> tile_renderables;


	void reset()
	{
		for (auto &ent : entities)
			ent.clear();

		entities.clear();
		physicals.clear();
		atlas_renderables.clear();
	}
};
