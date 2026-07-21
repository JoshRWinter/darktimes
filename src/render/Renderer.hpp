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
    struct StaticRenderable : Renderable
    {
        StaticRenderable(const Renderable &renderable, int index)
            : Renderable(renderable)
            , index(index)
        {
        }

        int index;
    };

public:
    explicit Renderer(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll);

    void set_leveldata(const LevelData &leveldata);
    void render(const Renderables &prev, const Renderables &current, float lerp);
    void resize(const win::Area<float> &area, const win::Dimensions<int> &res);

private:
    std::vector<int> static_renderables_staging;
    std::vector<Renderable> dynamic_renderables_staging;
    std::vector<int> static_lights_staging;
    std::vector<LightRenderable> dynamic_lights_staging;

    std::vector<StaticRenderable> static_renderables;
    std::vector<LightRenderable> static_lights;

    std::unique_ptr<RendererBackend> backend;
};
