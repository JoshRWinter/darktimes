#pragma once

#include <win/Win.hpp>

#include "../component/Component.hpp"

class Entity
{
	WIN_NO_COPY_MOVE(Entity);

public:
	constexpr static int max_components = 4;

	// name MUST BE STATIC!!!
	explicit Entity(const char *name)
		: name(name) {}

	~Entity()
	{
		for (auto &component : components)
			if (component.occupied)
				win::bug("Live component on entity " + std::string(name));
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

		win::bug("Component slots full on entity " + std::string(name));
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

	template <typename T> T &rget()
	{
		for (auto &component : components)
			if (component.occupied && component.component->type == T::ctype)
				return *(T*)component.component;

		win::bug("No component with type " + std::to_string((int)T::ctype) + " on entity " + name);
	}

	template <typename T> T *get()
	{
		for (auto &component : components)
			if (component.occupied && component.component->type == T::ctype)
				return (T*)component.component;

		return NULL;
	}

	void clear()
	{
		for (auto &component : components)
			component.occupied = false;
	}

private:
	const char *name;
	struct { Component *component = NULL; bool occupied = false; } components[max_components];
};
