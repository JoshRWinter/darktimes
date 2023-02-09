#pragma once

#include "Component.hpp"

struct RenderableComponent : Component
{
	constexpr static ComponentType ctype = ComponentType::renderable;

	RenderableComponent(Entity &entity, int texture)
		: Component(ctype, entity)
		, texture(texture)
	{}

	int texture;
};
