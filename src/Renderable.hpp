#pragma once

#include <vector>

#include "Texture.hpp"

struct Renderable
{
    Renderable(Texture texture, float x, float y, float w, float h, float rot)
        : texture(texture)
        , x(x)
        , y(y)
        , w(w)
        , h(h)
        , rot(rot)
    {
    }

    Texture texture;
    float x;
    float y;
    float w;
    float h;
    float rot;
};

struct Renderables
{
    std::vector<Renderable> renderables;

    float centerx = 0.0f;
    float centery = 0.0f;

    void clear() { renderables.clear(); }
};
