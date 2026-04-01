#pragma once

#include <win/SpatialIndex.hpp>

#include "../PoolTypes.hpp"
#include "../../GameInput.hpp"
#include "../entity/Entity.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/RenderableComponent.hpp"
#include "../component/PlayerComponent.hpp"

void player_system(
	win::SpatialIndex<PhysicalComponent> &index,
	Pool<Entity> &entities,
	Pool<PhysicalComponent> &physicals,
	Pool<RenderableComponent> &renderables,
	Pool<PlayerComponent> &players,
	const GameInput &input
);
