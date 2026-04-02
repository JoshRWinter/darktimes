#pragma once

#include <win/Win.hpp>

#include "../component/Components.hpp"

class Entity
{
    WIN_NO_COPY_MOVE(Entity);

public:
    constexpr static int max_components = 4;

    // name MUST BE STATIC!!!
    explicit Entity(const char *name);
    ~Entity();

    Component &add(Component &c);
    Component *add(Component *c);
    void remove(ComponentType type);
    void clear();

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

class World;

struct PlayerEntity
{
    PlayerEntity() = delete;

    static constexpr float width = 0.4f;
    static constexpr float height = 0.4f;

    static void create(World &);
};
