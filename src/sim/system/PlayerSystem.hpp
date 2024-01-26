#pragma once

#include <win/Pool.hpp>

#include "../BlockMap.hpp"
#include "../../GameInput.hpp"
#include "../entity/Entity.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/RenderableComponent.hpp"
#include "../component/PlayerComponent.hpp"

void player_system(
	BlockMap<PhysicalComponent> &blockmap,
	win::Pool<Entity> &entities,
	win::Pool<PhysicalComponent> &physicals,
	win::Pool<RenderableComponent> &renderables,
	win::Pool<PlayerComponent> &players,
	const GameInput &input
);
