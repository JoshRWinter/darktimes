#pragma once

#include <win/Pool.hpp>
#include <win/BlockMap.hpp>

#include "../Darktimes.hpp"
#include "../GameInput.hpp"
#include "../RenderableWorldState.hpp"
#include "../levelgen/LevelObjects.hpp"

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
	~World();

	void reset(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, const std::vector<LevelProp> &props);
	void tick(const GameInput &input, RenderableWorldState &state);

private:
	win::BlockMap<PhysicalComponent> blockmap;
	win::Pool<Entity> entities;
	win::Pool<PhysicalComponent> physicals;
	win::Pool<RenderableComponent> renderables;
	win::Pool<PlayerComponent> players;
};
