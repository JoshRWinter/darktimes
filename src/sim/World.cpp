#include "World.hpp"
#include "system/PlayerSystem.hpp"

World::~World()
{
	for (auto &entity : entities)
		entity.clear();
}

void World::tick(const GameInput &input, RenderableWorldState &state)
{
	player_system(entities, physicals, renderables, players, input);

	// map renderables
	for (const auto &r : renderables)
	{
		const auto &phys = r.entity.get<PhysicalComponent>();
		state.renderables.emplace_back(r.texture, phys.x, phys.y, phys.w, phys.h, phys.rot);
	}

	const auto &player_physical = (*players.begin()).entity.get<PhysicalComponent>();
	state.centerx = player_physical.x;
	state.centery = player_physical.y;
}
