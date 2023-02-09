#pragma once

#include "Component.hpp"

struct PhysicalComponent : Component
{
	constexpr static ComponentType ctype = ComponentType::physical;

	PhysicalComponent(Entity &entity, float x, float y, float w, float h, float rot)
		: Component(ctype, entity)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
		, rot(rot)
	{}

	float x, y, w, h, rot;
};
