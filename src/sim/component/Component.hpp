#pragma once

#include <win/Win.hpp>

enum class ComponentType
{
	physical,
	renderable,
	player
};

class Entity;
struct Component
{
	WIN_NO_COPY_MOVE(Component);

	Component(ComponentType type, Entity &entity)
		: type(type)
		, entity(entity)
	{}

	ComponentType type;
	Entity &entity;
};
