#pragma once

#include <win/Pool.hpp>

#include "entity/Entity.hpp"
#include "component/Component.hpp"
#include "component/PhysicalComponent.hpp"
#include "component/RenderableComponent.hpp"
#include "component/PlayerComponent.hpp"

struct World
{
	~World() { clear(); }

	win::Pool<Entity> entities;
	win::Pool<PhysicalComponent> physicals;
	win::Pool<RenderableComponent> atlas_renderables;
	win::Pool<RenderableComponent> tile_renderables;
	win::Pool<PlayerComponent> players;

	float centerx = 0.0f, centery = 0.0f;

	void clear()
	{
		for (auto &ent : entities)
			ent.clear();

		entities.clear();
		physicals.clear();
		atlas_renderables.clear();
		players.clear();
	}
};
