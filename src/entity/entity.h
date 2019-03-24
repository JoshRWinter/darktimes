#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

namespace game { struct renderer; }

namespace ent
{

	struct entity
	{
		bool collide(const ent::entity&) const;

		float x, y, w, h, rot;
	};

	struct furnishing : ent::entity
	{
		int texture;
	};

	struct wall : ent::entity
	{
		static constexpr float THICKNESS = 0.2f;

		wall(float, float, float, float);

		static void render(game::renderer&, const std::vector<ent::wall>&);
	};

	struct room_neighbor_type
	{
		enum class ntype
		{
			LEFT, RIGHT, ABOVE, BELOW
		};
	
		int index;
		ntype type;
	};

	struct room : ent::entity
	{
		room(float xx, float yy, float ww, float hh) { x = xx; y = yy; w = ww; h = hh; rot = 0.0; }
	
		std::vector<ent::room_neighbor_type> neighbors;
	};

}

#endif
