#pragma once

#include <cmath>

#include "Component.hpp"

struct PhysicalComponent : Component
{
	static constexpr ComponentType ctype = ComponentType::physical;

	PhysicalComponent(Entity &entity, float x, float y, float w, float h, float rot)
		: Component(ctype, entity)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
		, rot(rot)
	{}

	bool collide(const PhysicalComponent &rhs) const
	{
		return x + w > rhs.x && x < rhs.x + rhs.w && y + h > rhs.y && y < rhs.y + rhs.h;
	}

	bool correct(const PhysicalComponent &rhs)
	{
		if (!collide(rhs))
			return false;

		const float left = fabsf(x - (rhs.x + rhs.w));
		const float right = fabsf((x + w) - rhs.x);
		const float bottom = fabsf(y - (rhs.y + rhs.h));
		const float top = fabsf((y + h) - rhs.y);

		const float smallest = std::min(left, std::min(right, std::min(bottom, top)));

		if (smallest == left)
			x = rhs.x + rhs.w;
		else if (smallest == right)
			x = rhs.x - w;
		else if (smallest == bottom)
			y = rhs.y + rhs.h;
		else if (smallest == top)
			y = rhs.y - h;
		else
			win::bug("collision correction mishap");

		return true;
	}

	float x, y, w, h, rot;
};
