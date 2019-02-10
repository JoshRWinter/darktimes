#include "../darktimes.h"

bool ent::entity::collide(const ent::entity &other) const
{
	return x + w > other.x && x < other.x + other.w && y + h > other.y && y < other.y + other.h;
}
