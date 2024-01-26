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

	float leftmost = INFINITY, rightmost = INFINITY, bottommost = INFINITY, topmost = INFINITY;
	for (const auto &wall : walls)
	{
		auto &ent = entities.add("wall");
		auto &phys = physicals.add(ent, wall.x, wall.y, wall.w, wall.h, 0.0f);

		if (leftmost == INFINITY || phys.x < leftmost)
			leftmost = phys.x;
		if (rightmost == INFINITY || phys.x + phys.w > rightmost)
			rightmost = phys.x + phys.w;
		if (bottommost == INFINITY || phys.y < bottommost)
			bottommost = phys.y;
		if (topmost == INFINITY || phys.y + phys.h > topmost)
			topmost = phys.y + phys.h;
	}

	if (leftmost == INFINITY || rightmost == INFINITY || bottommost == INFINITY || topmost == INFINITY)
		win::bug("Block map initialization woes");

	blockmap.reset(1.0f, leftmost, rightmost, bottommost, topmost);

	for (auto &phys : physicals)
		blockmap.add(BlockMapLocation(phys.x, phys.y, phys.w, phys.h), phys);
}

void World::tick(const GameInput &input, RenderableWorldState &state)
{
	if (physicals.size() == 0)
		return; // world not initialized yet

	player_system(blockmap, entities, physicals, renderables, players, input);

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
