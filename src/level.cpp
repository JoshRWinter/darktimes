#include "darktimes.h"

struct room
{
	room(float xx, float yy, float ww, float hh) : x(xx), y(yy), w(ww), h(hh) {}

	bool collide(const room &other) const
	{ return x + w > other.x && x < other.x + other.w && y + h > other.y && y < other.y + other.h; }

	float x, y, w, h;
};

static room build_room(const room*);
static std::vector<ent::wall> build_walls(const std::vector<room>&);
static bool no_collisions(const std::vector<room>&, const room&);

void game::level::build_level(std::vector<ent::wall> &walls, std::vector<ent::furnishing> &furnishings)
{
	std::vector<room> rooms;

	// first room
	rooms.push_back(build_room(NULL));
	int prev = 0;

	const int iterate = mersenne(14, 25);

	int tries = 100;
	while(rooms.size() < iterate)
	{
		if(--tries < 0)
		{
			// recurse, try again
			build_level(walls, furnishings);
			return;
		}

		const int children = mersenne(0, 5);

		for(int j = 0; j < children; ++j)
		{
			const room new_room = build_room(&rooms[prev]);
			if(no_collisions(rooms, new_room))
				rooms.push_back(new_room);
		}

		prev = rooms.size() - 1;
	}

	const std::vector<ent::wall> built = build_walls(rooms);
	dput("made {} rooms", rooms.size());
	walls = built;
}

std::vector<ent::wall> build_walls(const std::vector<room>& rooms)
{
	std::vector<ent::wall> walls;

	const float half = ent::wall::THICKNESS / 2.0f;

	for(const room &rm : rooms)
	{
		// top
		walls.push_back(ent::wall(rm.x - half, rm.y + rm.h - half, rm.w + ent::wall::THICKNESS, ent::wall::THICKNESS));
		// bottom
		walls.push_back(ent::wall(rm.x - half, rm.y - half, rm.w + ent::wall::THICKNESS, ent::wall::THICKNESS));
		// right
		walls.push_back(ent::wall(rm.x + rm.w - half, rm.y - half, ent::wall::THICKNESS, rm.h + ent::wall::THICKNESS));
		// left
		walls.push_back(ent::wall(rm.x - half, rm.y - half, ent::wall::THICKNESS, rm.h + ent::wall::THICKNESS));
	}

	return walls;
}

room build_room(const room *parent)
{
	if(parent == NULL)
	{
		return room(-2.0f, -2.0f, 4.0f, 4.0f);
	}

	const int side = mersenne(0, 3);

	const float width = mersenne(4.0f, 10.0f);
	const float height = mersenne(4.0f, 10.0f);

	const float width_offset = mersenne(-width / 3.0f, width / 3.0f);
	const float height_offset = mersenne(-height / 3.0f, height / 3.0f);

	switch(side)
	{
		case 0: // above
			return room(parent->x + width_offset, parent->y + parent->h, width, height);
		case 1: // below
			return room(parent->x + width_offset, parent->y - height, width, height);
		case 2: // right
			return room(parent->x + parent->w, parent->y + height_offset, width, height);
		case 3: // left
			return room(parent->x - width, parent->y + height_offset, width, height);
		default:
			win::bug("no side");
	}
}

bool no_collisions(const std::vector<room> &rooms, const room &candidate)
{
	for(const room &rm : rooms)
	{
		if(candidate.collide(rm))
			return false;
	}

	return true;
}
