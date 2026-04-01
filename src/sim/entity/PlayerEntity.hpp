#pragma once

#include "../PoolTypes.hpp"
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
		Pool<Entity>&,
		Pool<PhysicalComponent>&,
		Pool<RenderableComponent>&,
		Pool<PlayerComponent>&
	);
};
