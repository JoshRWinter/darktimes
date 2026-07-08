#pragma once

#include <memory>
#include <vector>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "../LevelData.hpp"
#include "../Renderable.hpp"
#include "RendererBackend.hpp"

class Renderer
{
public:
    explicit Renderer(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll);

    void set_leveldata(const LevelData &leveldata);
    void render(const Renderables &prev, const Renderables &next, float lerp);
    void resize(const win::Area<float> &area, const win::Dimensions<int> &res);

private:
    std::vector<int> static_renderable_staging;
    std::vector<int> static_light_staging;

    std::vector<Renderable> static_renderables;
    std::vector<LightRenderable> static_lights;

    std::unique_ptr<RendererBackend> backend;
};
