#ifndef WORLD_H
#define WORLD_H

namespace game
{

struct world
{
	world();
	void step();
	void render(game::renderer&);
};

}

#endif
