#pragma once

#include <win/Pool.hpp>

#include "../Darktimes.hpp"
#include "../GameInput.hpp"
#include "../RenderableWorldState.hpp"

#include "entity/Entity.hpp"
#include "component/Component.hpp"
#include "component/PhysicalComponent.hpp"
#include "component/RenderableComponent.hpp"
#include "component/PlayerComponent.hpp"

class World
{
	NO_COPY_MOVE(World);

public:
	World() = default;

	RenderableWorldState tick(const GameInput &input);

private:
	win::Pool<Entity> entities;
	win::Pool<PhysicalComponent> physicals;
	win::Pool<RenderableComponent> atlas_renderables;
	win::Pool<RenderableComponent> tile_renderables;
	win::Pool<PlayerComponent> players;

	float centerx = 0.0f, centery = 0.0f;
};
