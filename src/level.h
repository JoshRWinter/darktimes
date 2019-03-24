#ifndef LEVEL_H
#define LEVEL_H

namespace game
{

	struct level
	{
		void reset();

		std::vector<ent::room> rooms;
		std::vector<ent::wall> walls;
		std::vector<ent::furnishing> non_colliding_furnishings;
		std::vector<ent::furnishing> colliding_furnishings;
	};

}

#endif
