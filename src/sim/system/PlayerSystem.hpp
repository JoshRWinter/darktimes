#pragma once

#include <win/SpatialIndex.hpp>

#include "../../GameInput.hpp"
#include "../component/PhysicalComponent.hpp"
#include "../component/PlayerComponent.hpp"
#include "../component/RenderableComponent.hpp"
#include "../entity/Entity.hpp"
#include "../PoolTypes.hpp"

void player_system(win::SpatialIndex<PhysicalComponent> &index,
                   Pool<Entity> &entities,
                   Pool<PhysicalComponent> &physicals,
                   Pool<RenderableComponent> &renderables,
                   Pool<PlayerComponent> &players,
                   const GameInput &input);
