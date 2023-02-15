#pragma once

#include <win/Pool.hpp>

#include "../entity/Entity.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/RenderableComponent.hpp"
#include "../component/PlayerComponent.hpp"
#include "../../Input.hpp"

void player_system(
	win::Pool<Entity>&,
	win::Pool<PhysicalComponent>&,
	win::Pool<RenderableComponent>&,
	win::Pool<PlayerComponent>&,
	const Input&,
	float&,
	float&
);
