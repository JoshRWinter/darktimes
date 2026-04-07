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

    float centerx;
    float centery;

    void clear()
    {
        centerx = 0.0f;
        centery = 0.0f;
        renderables.clear();
    }
};
