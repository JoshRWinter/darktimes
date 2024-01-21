#pragma once

#include <win/Pool.hpp>

#include "Entity.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/RenderableComponent.hpp"
#include "../component/PlayerComponent.hpp"

struct PlayerEntity
{
	PlayerEntity() = delete;

	static constexpr float width = 0.4f;
	static constexpr float height = 0.4f;

	static void create(
		win::Pool<Entity>&,
		win::Pool<PhysicalComponent>&,
		win::Pool<RenderableComponent>&,
		win::Pool<PlayerComponent>&
	);
};
