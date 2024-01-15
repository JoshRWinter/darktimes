#pragma once

#include <win/Pool.hpp>

#include "../entity/Entity.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/RenderableComponent.hpp"
#include "../component/PlayerComponent.hpp"

void player_system(
	win::Pool<Entity>&,
	win::Pool<PhysicalComponent>&,
	win::Pool<RenderableComponent>&,
	win::Pool<PlayerComponent>&,
	float&,
	float&
);
