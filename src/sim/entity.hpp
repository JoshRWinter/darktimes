#pragma once

#include <win/win.hpp>

#include "component.hpp"

class Entity
{
	WIN_NO_COPY_MOVE(Entity);

public:
	constexpr static int max_components = 4;

	Entity() = default;

	Component &add(Component &c) { return *add(&c); }
	Component *add(Component *c)
	{
		for (int i = 0; i < max_components; ++i)
		{
			if (!components[i].occupied)
			{
				components[i].occupied = true;
				components[i].component = c;

				return c;
			}
		}

		win::bug("Component slots full");
	}

	void remove(ComponentType type)
	{
		for (int i = 0; i < max_components; ++i)
		{
			if (components[i].occupied && components[i].component->type == type)
			{
				components[i].occupied = false;
				components[i].component = NULL;
			}
		}
	}

	template <typename T> T *get() { return get<T>(T::ctype); }
	template <typename T> T *get(ComponentType type)
	{
		for (int i = 0; i < max_components; ++i)
		{
			if (components[i].occupied && components[i].component->type == type)
				return (T*)components[i].component;
		}

		return NULL;
	}

private:
	struct { Component *component = NULL; bool occupied = false; } components[max_components];
};
