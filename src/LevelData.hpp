#pragma once

#include "Renderable.hpp"

struct LevelData
{
    std::vector<Renderable> renderables;
    std::vector<LightOccluder> occluders;
    std::vector<LightRenderable> lights;
};
