#pragma once

#include <win/Win.hpp>

#include "../component/Components.hpp"

class Entity
{
    WIN_NO_COPY_MOVE(Entity);

public:
    constexpr static int max_components = 4;

    // name MUST BE STATIC!!!
    explicit Entity(const char *name)
        : name(name)
    {
    }

    ~Entity()
    {
        for (const auto &component : components)
            if (component.occupied)
                win::bug("Live component on entity " + std::string(name));
    }

    template<typename T> T &add(T &c) { return *add(&c); }

    template<typename T> T *add(T *c)
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

    template<typename T> T &remove()
    {
        for (auto &component : components)
        {
            if (component.occupied && component.component->type == T::ctype)
            {
                component.occupied = false;
                auto &c = *component.component;
                component.component = NULL;
                return (T &)c;
            }
        }

        win::bug("No component of type " + std::to_string((int)T::ctype));
    }

    template<typename T> T &get()
    {
        for (auto &component : components)
            if (component.occupied && component.component->type == T::ctype)
                return *(T *)component.component;

        win::bug("No component with type " + std::to_string((int)T::ctype) + " on entity " + name);
    }

    template<typename T> T *get_optional()
    {
        for (auto &component : components)
            if (component.occupied && component.component->type == T::ctype)
                return (T *)component.component;

        return NULL;
    }

private:
    const char *name;

    struct
    {
        Component *component = NULL;
        bool occupied = false;
    } components[max_components];
};

struct World;

struct PlayerEntity
{
    PlayerEntity() = delete;

    static constexpr float width = 0.4f;
    static constexpr float height = 0.4f;

    static void create(World &world);
    static void destroy(World &world, Entity &entity);
};
