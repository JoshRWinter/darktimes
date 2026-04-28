#pragma once

#include <cmath>
#include <vector>

#include "LevelObjectsInternal.hpp"

struct LevelPropDefinition
{
    static constexpr float default_excluder_padding = 0.2f;

    LevelPropDefinition(Texture texture,
                        bool solid,
                        int width,
                        int height,
                        float excluder_padding_x = default_excluder_padding,
                        float excluder_padding_y = default_excluder_padding)
        : LevelPropDefinition(texture, solid, (width / 1920.0f) * 16.0f, (height / 1080.0f) * 9.0f, excluder_padding_x, excluder_padding_y)
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

    LevelPropInternal spawn(LevelSide side, float x, float y) const
    {
        float w, h;
        float xpad, ypad;
        switch (side)
        {
            case LevelSide::left:
            case LevelSide::right:
                w = width;
                h = height;
                xpad = excluder_padding_x;
                ypad = excluder_padding_y;
                break;
            case LevelSide::bottom:
            case LevelSide::top:
                w = height;
                h = width;
                xpad = excluder_padding_y;
                ypad = excluder_padding_x;
                break;
        }

        return LevelPropInternal(texture, side, solid, x, y, w, h, xpad, ypad);
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
};

struct PropDefinitions
{
    static const PropDefinitions &get()
    {
        static PropDefinitions pd;
        return pd;
    }

    std::vector<LevelPropDefinition> side_furniture = {
        LevelPropDefinition(Texture::chair1, true, 58, 82, 0.18f, 0.0f),      LevelPropDefinition(Texture::chair2, true, 58, 82, 0.18f, 0.0f),
        LevelPropDefinition(Texture::couch1, true, 58, 136, 0.18f, 0.0f),     LevelPropDefinition(Texture::side_table1, true, 62, 130, 0.18f, 0.0f),
        LevelPropDefinition(Texture::side_table2, true, 45, 64, 0.18f, 0.0f), LevelPropDefinition(Texture::side_shelf, true, 44, 88, 0.18f, 0.0f),
        LevelPropDefinition(Texture::piano, true, 50, 112, 0.18f, 0.0f)
    };

    std::vector<LevelPropDefinition> center_tables = { LevelPropDefinition(Texture::large_table, true, 106, 190, 0.18f, 0.25f),
                                                       LevelPropDefinition(Texture::large_table2, true, 106, 160, 0.18f, 0.25f) };

    std::vector<LevelPropDefinition> huge_center_tables = {
        LevelPropDefinition(Texture::huge_table, true, 286, 286, 0.25f, 0.25f),
        LevelPropDefinition(Texture::long_table, true, 184, 394, 0.25f, 0.25f),
    };

    std::vector<LevelPropDefinition> rugs = { LevelPropDefinition(Texture::rug1, false, 244, 124, 0.0f, 0.0f),
                                              LevelPropDefinition(Texture::rug2, false, 244, 163, 0.0f, 0.0f),
                                              LevelPropDefinition(Texture::rug3, false, 304, 88, 0.0f, 0.0f),
                                              LevelPropDefinition(Texture::rug4, false, 220, 144, 0.0f, 0.0f) };

    std::vector<LevelPropDefinition> clutter = { LevelPropDefinition(Texture::clutter_brokenglass, false, 44, 40, 0.0f, 0.0f),
                                                 LevelPropDefinition(Texture::clutter_boot, false, 38, 32, 0.0f, 0.0f) };

    LevelPropDefinition floor_transition_strip = LevelPropDefinition(Texture::transition_strip, false, 0.025f, 1.0f, 0.0f, 0.0f);

private:
    PropDefinitions() = default;
};
