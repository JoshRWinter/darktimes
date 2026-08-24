#pragma once

#include <vector>

#include "LevelObjectsInternal.hpp"

struct LevelPropDefinition
{
    static constexpr float default_excluder_padding = 0.2f;

    LevelPropDefinition(Texture texture,
                        bool solid,
                        int width,
                        int height,
                        int shadowpad = 0,
                        float excluder_padding_x = default_excluder_padding,
                        float excluder_padding_y = default_excluder_padding)
        : LevelPropDefinition(texture,
                              solid,
                              ((width + shadowpad) / 1920.0f) * 16.0f,
                              ((height + shadowpad) / 1080.0f) * 9.0f,
                              excluder_padding_x,
                              excluder_padding_y)
    {
    }

    LevelPropDefinition(Texture texture,
                        bool solid,
                        float width,
                        float height,
                        float excluder_padding_x = default_excluder_padding,
                        float excluder_padding_y = default_excluder_padding)
        : texture(texture)
        , solid(solid)
        , excluder_padding_x(excluder_padding_x)
        , excluder_padding_y(excluder_padding_y)
        , width(width)
        , height(height)
    {
    }

    LevelPropInternal spawn(LevelSide side, float x, float y) const { return spawn(side, x, y, 0.0f, win::Color<float>()); }

    LevelPropInternal spawn(LevelSide side, float x, float y, float lightpower, const win::Color<float> &lightcolor) const
    {
        float w = 0.0f, h = 0.0f;
        float xpad = 0.0f, ypad = 0.0f;
        float light_x = 0.0f, light_y = 0.0f;
        switch (side)
        {
            case LevelSide::left:
            case LevelSide::right:
                w = width;
                h = height;
                xpad = excluder_padding_x;
                ypad = excluder_padding_y;
                light_x = side == LevelSide::right ? lightx : (width - lightx);
                light_y = side == LevelSide::right ? lighty : (height - lighty);
                break;
            case LevelSide::bottom:
            case LevelSide::top:
                w = height;
                h = width;
                xpad = excluder_padding_y;
                ypad = excluder_padding_x;
                light_x = side == LevelSide::bottom ? lighty : (height - lighty);
                light_y = side == LevelSide::bottom ? (width - lightx) : lightx;
                break;
        }

        return LevelPropInternal(texture,
                                 side,
                                 solid,
                                 x,
                                 y,
                                 w,
                                 h,
                                 xpad,
                                 ypad,
                                 x + light_x,
                                 y + light_y,
                                 has_light ? lightpower : 0.0f,
                                 has_light ? lightcolor : win::Color<float>());
    }

    LevelPropDefinition &set_light(int x, int y)
    {
        has_light = true;
        lightx = x / 1920.0f * 16.0f;
        lighty = y / 1080.0f * 9.0f;
        return *this;
    }

    float get_width(LevelSide side) const { return side == LevelSide::left || side == LevelSide::right ? width : height; }

    float get_height(LevelSide side) const { return side == LevelSide::left || side == LevelSide::right ? height : width; }

private:
    Texture texture;
    bool solid;
    float excluder_padding_x;
    float excluder_padding_y;
    float width;
    float height;
    bool has_light = false;
    float lightx = 0.0f;
    float lighty = 0.0f;
};

struct PropDefinitions
{
    static const PropDefinitions &get()
    {
        static PropDefinitions pd;
        return pd;
    }

    // clang-format off

    std::vector<LevelPropDefinition> side_furniture =
    {
        LevelPropDefinition(Texture::chair1, true, 48, 71, 12, 0.18f, 0.0f),
        LevelPropDefinition(Texture::chair2, true, 48, 71, 12, 0.18f, 0.0f),
        LevelPropDefinition(Texture::couch1, true, 48, 126, 12, 0.18f, 0.0f),
        LevelPropDefinition(Texture::side_table1, true, 49, 120, 12, 0.18f, 0.0f).set_light(16, 16),
        LevelPropDefinition(Texture::side_table2, true, 35, 55, 12, 0.18f, 0.0f),
        LevelPropDefinition(Texture::side_shelf, true, 47, 110, 12, 0.18f, 0.0f),
        LevelPropDefinition(Texture::piano, true, 47, 150, 12, 0.18f, 0.0f),
        LevelPropDefinition(Texture::lamp, true, 35, 35, 12, 0.18f, 0.0f).set_light(22, 22)
    }   ;

    std::vector<LevelPropDefinition> center_tables =
    {
        LevelPropDefinition(Texture::large_table, true, 96, 179, 18, 0.18f, 0.25f),
        LevelPropDefinition(Texture::large_table2, true, 96, 96, 18, 0.18f, 0.25f)
    };

    std::vector<LevelPropDefinition> huge_center_tables =
    {
        LevelPropDefinition(Texture::huge_table, true, 276, 276, 18, 0.25f, 0.25f),
        LevelPropDefinition(Texture::long_table, true, 174, 384, 18, 0.25f, 0.25f),
    };

    std::vector<LevelPropDefinition> rugs =
    {
        LevelPropDefinition(Texture::rug1, false, 240, 122, 4, 0.0f, 0.0f),
        LevelPropDefinition(Texture::rug2, false, 240, 125, 4, 0.0f, 0.0f),
        LevelPropDefinition(Texture::rug3, false, 300, 84, 4, 0.0f, 0.0f),
        LevelPropDefinition(Texture::rug4, false, 216, 140, 4, 0.0f, 0.0f)
    };

    std::vector<LevelPropDefinition> clutter =
    {
        LevelPropDefinition(Texture::clutter_brokenglass, false, 36, 32, 6, 0.0f, 0.0f),
        LevelPropDefinition(Texture::clutter_boot, false, 30, 24, 6, 0.0f, 0.0f),
        LevelPropDefinition(Texture::clutter_bottle, false, 25, 38, 6, 0.0f, 0.0f)
    };

    LevelPropDefinition floor_transition_strip = LevelPropDefinition(Texture::transition_strip, false, 0.025f, 1.0f, 0.0f, 0.0f);

    // clang-format on

private:
    PropDefinitions() = default;
};
