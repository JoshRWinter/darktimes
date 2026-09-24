#pragma once

#include <win/Bag.hpp>
#include <win/ComponentSet.hpp>
#include <win/Win.hpp>

#include "../component/Components.hpp"

constexpr float pixelsize(int s, int shadowpad = 0)
{
    return (s + shadowpad) / 1920.0f * 16.0f;
}

class Entity
{
    WIN_NO_COPY_MOVE(Entity);

public:
    typedef win::Bag<win::ComponentChunk<Component>, 100, true> ComponentBag;

    // name MUST BE STATIC!!!
    Entity(ComponentBag &bag, const char *name)
        : name(name)
        , components(bag)
    {
    }

    ~Entity()
    {
        if (components.size() != 0)
            win::bug("Live component on entity " + std::string(name));
    }

    const char *const name;
    win::ComponentSet<Component, ComponentBag> components;
};

struct World;

struct PlayerEntity
{
    PlayerEntity() = delete;

    static constexpr float width = 0.4f;
    static constexpr float height = 0.4f;

    static constexpr float torso_width = pixelsize(22, 6);
    static constexpr float torso_height = pixelsize(49, 6);

    static constexpr float leg_width = pixelsize(29, 4);
    static constexpr float leg_height = pixelsize(10, 4);

    static constexpr float head_width = pixelsize(20, 4);
    static constexpr float head_height = pixelsize(20, 4);

    static void create(World &world);
    static void destroy(World &world, Entity &entity);
};
