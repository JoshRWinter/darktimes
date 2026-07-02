#pragma once

#include <win/Utility.hpp>

#include "../../Texture.hpp"

enum class LevelSide
{
    left,
    right,
    bottom,
    top
};

struct LevelLight
{
    LevelLight(float x, float y, float power, const win::Color<float> &color, float angle)
        : x(x)
        , y(y)
        , power(power)
        , color(color)
        , angle(angle)
    {
    }

    float x;
    float y;
    float power;
    win::Color<float> color;
    float angle;
};

struct LevelProp
{
    LevelProp(Texture texture,
              LevelSide side,
              bool solid,
              float x,
              float y,
              float w,
              float h,
              float lightx,
              float lighty,
              float lightpower,
              const win::Color<float> &lightcolor)
        : texture(texture)
        , side(side)
        , solid(solid)
        , x(x)
        , y(y)
        , w(w)
        , h(h)
        , lightx(lightx)
        , lighty(lighty)
        , lightpower(lightpower)
        , lightcolor(lightcolor)
    {
    }

    Texture texture;
    LevelSide side;
    bool solid;
    float x, y, w, h;
    float lightx, lighty, lightpower;
    win::Color<float> lightcolor;
};

struct LevelWall
{
    LevelWall(float x, float y, float w, float h)
        : x(x)
        , y(y)
        , w(w)
        , h(h)
    {
    }

    float x, y, w, h;
};

struct LevelFloor
{
    LevelFloor(Texture texture, float x, float y, float w, float h)
        : texture(texture)
        , x(x)
        , y(y)
        , w(w)
        , h(h)
    {
    }

    Texture texture;
    float x, y, w, h;
};
