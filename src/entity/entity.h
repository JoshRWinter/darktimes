#ifndef ENTITY_H
#define ENTITY_H

namespace game { struct renderer; }

namespace ent
{

	struct entity
	{
		bool collide(const ent::entity&) const;

		float x, y, w, h, rot;
	};

	struct wall : ent::entity
	{
		static constexpr float THICKNESS = 0.375f;

		wall(float, float, float, float);

		static std::vector<ent::wall> generate();
		static void render(game::renderer&, const std::vector<ent::wall>&);
	};

}

#endif
