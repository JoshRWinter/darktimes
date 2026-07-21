#pragma once

#include <vector>

#include <win/Utility.hpp>

#include "Texture.hpp"

struct Renderable
{
    Renderable(int id, Texture texture, float x, float y, float w, float h, float rot)
        : id(id)
        , texture(texture)
        , x(x)
        , y(y)
        , w(w)
        , h(h)
        , rot(rot)
    {
    }

    int id;
    Texture texture;
    float x;
    float y;
    float w;
    float h;
    float rot;
};

struct LightRenderable
{
    LightRenderable(int id, float x, float y, float power, const win::Color<float> &color, float angle, bool primary)
        : id(id)
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
