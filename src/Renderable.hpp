#pragma once

#include <vector>

#include <win/Utility.hpp>

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

struct LightRenderable
{
    LightRenderable(float x, float y, float power, const win::Color<float> &color)
        : x(x)
        , y(y)
        , power(power)
        , color(color)
    {
    }

    float x;
    float y;
    float power;
    win::Color<float> color;
};

struct LightOccluder
{
    LightOccluder(float x1, float y1, float x2, float y2)
        : x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2)
    {
    }

    float x1;
    float y1;
    float x2;
    float y2;
};

struct Renderables
{
    std::vector<Renderable> renderables;
    std::vector<LightRenderable> light_renderables;

    float centerx = 0.0f;
    float centery = 0.0f;

    void clear()
    {
        renderables.clear();
        light_renderables.clear();
    }
};
