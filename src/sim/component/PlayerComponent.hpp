#pragma once

#include "Component.hpp"

struct PlayerComponent : Component
{
	static constexpr ComponentType ctype = ComponentType::player;

	PlayerComponent(Entity &entity)
		: Component(ctype, entity)
	{
	}
};
