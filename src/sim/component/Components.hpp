#pragma once

#include <cmath>

#include <win/Utility.hpp>
#include <win/Win.hpp>

#include "../../Texture.hpp"

enum class ComponentType
{
    physical,
    renderable,
    light_renderable,
    player,
    body
};

class Entity;

struct Component
{
    WIN_NO_COPY_MOVE(Component);

    Component(ComponentType type, Entity &entity)
        : entity(entity)
        , type(type)
    {
    }

    Entity &entity;
    ComponentType type;
};

struct PhysicalComponent : Component
{
    static constexpr auto ctype = ComponentType::physical;

    PhysicalComponent(Entity &entity, float x, float y, float w, float h, float rot)
        : Component(ctype, entity)
        , x(x)
        , y(y)
        , w(w)
        , h(h)
        , rot(rot)
    {
    }

    bool collide(const PhysicalComponent &rhs) const { return x + w > rhs.x && x < rhs.x + rhs.w && y + h > rhs.y && y < rhs.y + rhs.h; }

    bool correct(const PhysicalComponent &rhs)
    {
        if (!collide(rhs))
            return false;

        const float left = fabsf(x - (rhs.x + rhs.w));
        const float right = fabsf((x + w) - rhs.x);
        const float bottom = fabsf(y - (rhs.y + rhs.h));
        const float top = fabsf((y + h) - rhs.y);

        const float smallest = std::min(left, std::min(right, std::min(bottom, top)));

        if (smallest == left)
            x = rhs.x + rhs.w;
        else if (smallest == right)
            x = rhs.x - w;
        else if (smallest == bottom)
            y = rhs.y + rhs.h;
        else if (smallest == top)
            y = rhs.y - h;
        else
            win::bug("collision correction mishap");

        return true;
    }

    float x, y, w, h, rot;
};

struct RenderableComponent : Component
{
    static constexpr auto ctype = ComponentType::renderable;

    RenderableComponent(Entity &entity, int id, float w, float h, Texture texture)
        : Component(ctype, entity)
        , id(id)
        , w(w)
        , h(h)
        , texture(texture)
    {
    }

    RenderableComponent(Entity &entity, int id, float x, float y, float w, float h, Texture texture)
        : Component(ctype, entity)
        , id(id)
        , x(x)
        , y(y)
        , w(w)
        , h(h)
        , texture(texture)
    {
    }

    void from_physical(const PhysicalComponent &phys)
    {
        x = phys.x + phys.w / 2.0f - w / 2.0f;
        y = phys.y + phys.h / 2.0f - h / 2.0f;
        rot = phys.rot;
    }

    int id;
    float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, rot = 0.0f;
    Texture texture;
};

struct LightRenderableComponent : Component
{
    static constexpr auto ctype = ComponentType::light_renderable;

    LightRenderableComponent(Entity &entity, int id, float x, float y, float power, const win::Color<float> &color, float angle, bool primary)
        : Component(ctype, entity)
        , id(id)
        , x(x)
        , y(y)
        , power(power)
        , color(color)
        , angle(angle)
        , primary(primary)
    {
    }

    int id;
    float x;
    float y;
    float power;
    win::Color<float> color;
    float angle;
    bool primary;
};

struct PlayerComponent : Component
{
    static constexpr auto ctype = ComponentType::player;

    explicit PlayerComponent(Entity &entity)
        : Component(ctype, entity)
    {
    }
};

struct BodyComponent : Component
{
    static constexpr auto ctype = ComponentType::body;

    BodyComponent(Entity &ent, RenderableComponent &head, RenderableComponent &torso, RenderableComponent &left, RenderableComponent &right)
        : Component(ctype, ent)
        , head(head)
        , torso(torso)
        , left(left)
        , right(right)
    {
    }

    RenderableComponent &head;
    RenderableComponent &torso;
    RenderableComponent &left;
    RenderableComponent &right;

    bool moving = false;

    float speed = 0.0f;
    int cycle = 0;
};
