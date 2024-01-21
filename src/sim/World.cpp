#include "World.hpp"
#include "system/PlayerSystem.hpp"

#include "entity/PlayerEntity.hpp"

World::~World()
{
	for (auto &entity : entities)
		entity.clear();
}

void World::reset(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, const std::vector<LevelProp> &props)
{
	for (auto &entity : entities)
		entity.clear();

	entities.clear();
	physicals.clear();
	renderables.clear();
	players.clear();

	for (const auto &wall : walls)
	{
		auto &ent = entities.add("wall");
		physicals.add(ent, wall.x, wall.y, wall.w, wall.h, 0.0f);
	}
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
	state.centerx = player_physical.x + (PlayerEntity::width / 2.0f);
	state.centery = player_physical.y + (PlayerEntity::height / 2.0f);
}
