#pragma once

#include <win/Win.hpp>

#include "component/Component.hpp"

class Entity
{
	WIN_NO_COPY_MOVE(Entity);

public:
	constexpr static int max_components = 4;

	Entity() = default;

	~Entity()
	{
		for (auto &component : components)
			if (component.occupied)
				win::bug("Live component on entity");
	}

	Component &add(Component &c) { return *add(&c); }
	Component *add(Component *c)
	{
		for (auto &component : components)
		{
			if (!component.occupied)
			{
				component.occupied = true;
				component.component = c;

				return c;
			}
		}

		win::bug("Component slots full");
	}

	void remove(ComponentType type)
	{
		for (auto &component : components)
		{
			if (component.occupied && component.component->type == type)
			{
				component.occupied = false;
				component.component = NULL;
			}
		}
	}

	template <typename T> T *get() { return get<T>(T::ctype); }
	template <typename T> T *get(ComponentType type)
	{
		for (auto &component : components)
		{
			if (component.occupied && component.component->type == type)
				return (T*)component.component;
		}

		return NULL;
	}

	void clear()
	{
		for (auto &component : components)
			component.occupied = false;
	}

private:
	struct { Component *component = NULL; bool occupied = false; } components[max_components];
};
