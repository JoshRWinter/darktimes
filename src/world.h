#ifndef WORLD_H
#define WORLD_H

namespace game
{

struct world
{
	world();

	void step();
	void render(game::renderer&);
	void reset();

	struct
	{
		std::vector<ent::wall> walls;
		std::vector<ent::furnishing> furnishings;
	} entity;
};

}

#endif
