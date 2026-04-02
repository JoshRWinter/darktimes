#include "Entities.hpp"

Entity::Entity(const char *name)
    : name(name)
{
}

Entity::~Entity()
{
    for (auto &component : components)
        if (component.occupied)
            win::bug("Live component on entity " + std::string(name));
}

Component &Entity::add(Component &c)
{
    return *add(&c);
}

Component *Entity::add(Component *c)
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

void Entity::remove(ComponentType type)
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

void Entity::clear()
{
    for (auto &component : components)
        component.occupied = false;
}
