#pragma once

#include <win/win.hpp>

enum class ComponentType
{
	physical,
	renderable
};

class Entity;
struct Component
{
	WIN_NO_COPY_MOVE(Component);

public:
	explicit Component(ComponentType type, Entity &entity)
		: type(type)
		, entity(entity)
	{}

	ComponentType type;
	Entity &entity;
};

struct PhysicalComponent : public Component
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

struct RenderableComponent : public Component
{
	constexpr static ComponentType ctype = ComponentType::renderable;

	RenderableComponent(Entity &entity, int texture)
		: Component(ctype, entity)
		, texture(texture)
	{}

	int texture;
};
