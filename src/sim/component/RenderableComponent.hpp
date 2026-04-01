#pragma once

#include "../../Texture.hpp"
#include "Component.hpp"

struct RenderableComponent : Component
{
    static constexpr ComponentType ctype = ComponentType::renderable;

    RenderableComponent(Entity &entity, Texture texture)
        : Component(ctype, entity)
        , texture(texture)
    {
    }

    Texture texture;
};
