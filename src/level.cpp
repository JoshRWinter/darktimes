#include "darktimes.h"

static ent::room build_room(const ent::room*, ent::room_neighbor_type::ntype*);
static std::vector<ent::wall> build_walls(const std::vector<ent::room>&);
static ent::room_neighbor_type::ntype opposite(ent::room_neighbor_type::ntype);
static void cut_doors(const std::vector<ent::room>&, std::vector<ent::wall>&);

void game::level::reset()
{
	const int target_count = mersenne(30, 35);
	int tries = 0;
	int current_room;

	while(rooms.size() < target_count)
	{
		if(--tries < 0)
		{
			// reset
			tries = 200;
			current_room = 0;

			rooms.clear();
			walls.clear();
			non_colliding_furnishings.clear();
			colliding_furnishings.clear();

			// first room
			rooms.push_back(build_room(NULL, NULL));
		}

		ent::room_neighbor_type::ntype type;
		ent::room candidate = build_room(&rooms.at(current_room), &type);

		// check for collisions with existing rooms
		bool collision = false;
		for(const ent::room &r : rooms)
		{
			if(r.collide(candidate))
			{
				collision = true;
				break;
			}
		}
		if(collision)
			continue;

		// tell the current room about its new neighbor
		ent::room_neighbor_type nt;
		nt.index = rooms.size();
		nt.type = type;
		rooms.at(current_room).neighbors.push_back(nt);

		ent::room_neighbor_type nt2;
		nt2.index = current_room;
		nt2.type = opposite(type);
		candidate.neighbors.push_back(nt2);

		rooms.push_back(candidate);

		if(mersenne(3))
		{
			if(++current_room >= rooms.size())
				current_room = 0;
		}
	}

	walls = build_walls(rooms);
}

ent::room build_room(const ent::room *parent, ent::room_neighbor_type::ntype *type)
{
	if(parent == NULL)
	{
		return ent::room(-2.0f, -2.0f, 4.0f, 4.0f);
	}

	const int side = mersenne(0, 3);

	const float width = mersenne(4.0f, 10.0f);
	const float height = mersenne(4.0f, 10.0f);

	const float width_offset = mersenne(-width / 3.5f, width / 3.5f);
	const float height_offset = mersenne(-height / 3.5f, height / 3.5f);

	switch(side)
	{
		case 0: // above
			*type = ent::room_neighbor_type::ntype::ABOVE;
			return ent::room(parent->x + width_offset, parent->y + parent->h + ent::wall::THICKNESS, width, height);
		case 1: // below
			*type = ent::room_neighbor_type::ntype::BELOW;
			return ent::room(parent->x + width_offset, parent->y - height - ent::wall::THICKNESS, width, height);
		case 2: // right
			*type = ent::room_neighbor_type::ntype::RIGHT;
			return ent::room(parent->x + parent->w + ent::wall::THICKNESS, parent->y + height_offset, width, height);
		case 3: // left
			*type = ent::room_neighbor_type::ntype::LEFT;
			return ent::room(parent->x - width - ent::wall::THICKNESS, parent->y + height_offset, width, height);
		default:
			win::bug("no side");
	}
}

ent::room_neighbor_type::ntype opposite(const ent::room_neighbor_type::ntype type)
{
	switch(type)
	{
		case ent::room_neighbor_type::ntype::ABOVE:
			return ent::room_neighbor_type::ntype::BELOW;
		case ent::room_neighbor_type::ntype::BELOW:
			return ent::room_neighbor_type::ntype::ABOVE;
		case ent::room_neighbor_type::ntype::LEFT:
			return ent::room_neighbor_type::ntype::RIGHT;
		case ent::room_neighbor_type::ntype::RIGHT:
			return ent::room_neighbor_type::ntype::LEFT;
		default: win::bug("no correct side");
	}
}

static constexpr float DOOR_WIDTH = 1.5f;

std::vector<ent::wall> build_walls(const std::vector<ent::room> &rooms)
{
	std::vector<ent::wall> walls;

	for(const ent::room &rm : rooms)
	{
		// left
		walls.push_back(ent::wall(rm.x - (ent::wall::THICKNESS / 2.0f), rm.y - (ent::wall::THICKNESS / 2.0f), ent::wall::THICKNESS, rm.h + ent::wall::THICKNESS));

		// right
		walls.push_back(ent::wall(rm.x + rm.w - (ent::wall::THICKNESS / 2.0f), rm.y - (ent::wall::THICKNESS / 2.0f), ent::wall::THICKNESS, rm.h + ent::wall::THICKNESS));

		// bottom
		walls.push_back(ent::wall(rm.x - (ent::wall::THICKNESS / 2.0f), rm.y - (ent::wall::THICKNESS / 2.0f), rm.w + ent::wall::THICKNESS, ent::wall::THICKNESS));

		// top
		walls.push_back(ent::wall(rm.x - (ent::wall::THICKNESS / 2.0f), rm.y + rm.h - (ent::wall::THICKNESS / 2.0f), rm.w + ent::wall::THICKNESS, ent::wall::THICKNESS));
	}

	cut_doors(rooms, walls);

	return walls;
}

void cut_doors(const std::vector<ent::room> &rooms, std::vector<ent::wall> &walls)
{
	for(const ent::room &rm : rooms)
	{
		for(const ent::room_neighbor_type &nt : rm.neighbors)
		{
			const bool side = nt.type == ent::room_neighbor_type::ntype::LEFT || nt.type == ent::room_neighbor_type::ntype::RIGHT;
			const ent::room &neighbor = rooms.at(nt.index);

			float start_overlap, end_overlap;

			if(side)
			{
				const float start_overlap = std::max(rm.y, neighbor.y);
				const float end_overlap = std::min(rm.y + rm.h, neighbor.y + neighbor.h);

				const float midpoint = (start_overlap + end_overlap) / 2.0f;
				const float xpos = nt.type == ent::room_neighbor_type::ntype::LEFT ? rm.x : (rm.x + rm.w);

				ent::wall door(xpos, midpoint - (DOOR_WIDTH / 2.0f), 0.1f, DOOR_WIDTH);

				for(int i = 0; i < walls.size(); ++i)
				{
					if(walls[i].collide(door))
					{
						ent::wall bottom(walls[i].x, walls[i].y, ent::wall::THICKNESS, door.y - walls[i].y);
						ent::wall top(walls[i].x, door.y + door.h, ent::wall::THICKNESS, (walls[i].y + walls[i].h) - (door.y + door.h));
						walls.push_back(bottom);
						walls.push_back(top);
						walls.erase(walls.begin() + i);
						break;
					}
				}
			}
			else
			{
				const float start_overlap = std::max(rm.x, neighbor.x);
				const float end_overlap = std::min(rm.x + rm.w, neighbor.x + neighbor.w);

				const float midpoint = (start_overlap + end_overlap) / 2.0f;
				const float ypos = nt.type == ent::room_neighbor_type::ntype::BELOW ? rm.y : (rm.y + rm.h);

				ent::wall door(midpoint - (DOOR_WIDTH / 2.0f), ypos, DOOR_WIDTH, 0.1f);

				for(int i = 0; i < walls.size(); ++i)
				{
					if(walls[i].collide(door))
					{
						ent::wall left(walls[i].x, walls[i].y, door.x - walls[i].x, ent::wall::THICKNESS);
						ent::wall right(door.x + door.w, walls[i].y, (walls[i].x + walls[i].w) - (door.x + door.w), ent::wall::THICKNESS);
						walls.push_back(left);
						walls.push_back(right);
						walls.erase(walls.begin() + i);
						break;
					}
				}
			}
		}
	}
}
