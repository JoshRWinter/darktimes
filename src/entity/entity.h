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

}

#endif
