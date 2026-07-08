#include <cmath>

#include "gl/GLRendererBackend.hpp"
#include "Renderer.hpp"

Renderer::Renderer(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll)
    : static_renderable_staging(100)
    , static_light_staging(10)
    , backend(new GLRendererBackend(area, res, roll))
{
}

void Renderer::set_leveldata(const LevelData &leveldata)
{
    static_renderables = leveldata.renderables;
    static_lights = leveldata.lights;

    backend->load_statics(static_renderables, leveldata.occluders, leveldata.lights);
}

void Renderer::render(const Renderables &prev, const Renderables &next, float lerp)
{
    const auto nearby = [&](const auto &item, float dist)
    {
        return std::max(std::abs(item.x - prev.centerx), std::abs(item.y - prev.centery)) < dist;
    };

    const auto interpolate = [lerp](float a, float b)
    {
        return a + (b - a) * lerp;
    };

    backend->set_view(interpolate(prev.centerx, next.centerx), interpolate(prev.centery, next.centery), 1.0f);

    static_renderable_staging.clear();
    for (const auto &r : static_renderables)
        static_renderable_staging.push_back(static_renderable_staging.size());

    static_light_staging.clear();
    for (int i = 0; i < static_lights.size(); ++i)
    {
        if (nearby(static_lights[i], 15.0f))
            static_light_staging.push_back(i);
    }

    backend->render(static_renderable_staging, prev.renderables, static_light_staging, prev.light_renderables);
}

void Renderer::resize(const win::Area<float> &area, const win::Dimensions<int> &res)
{
    backend->resize(area, res);
}
