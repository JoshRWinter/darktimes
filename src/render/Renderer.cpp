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

void Renderer::render(const Renderables &renderables)
{
    const auto nearby = [&renderables](const auto &item, float dist)
    {
        return std::max(std::abs(item.x - renderables.centerx), std::abs(item.y - renderables.centery)) < dist;
    };

    backend->set_view(renderables.centerx, renderables.centery, 1.0f); // 2.5f);
    backend->begin();

    static_renderable_staging.clear();
    for (const auto &r : static_renderables)
        static_renderable_staging.push_back(static_renderable_staging.size());

    backend->render_statics(static_renderable_staging);

    backend->render_dynamics(renderables.renderables);

    static_light_staging.clear();
    for (int i = 0; i < static_lights.size(); ++i)
    {
        if (nearby(static_lights[i], 15.0f))
            static_light_staging.push_back(i);
    }

    backend->render_lights(static_light_staging, renderables.light_renderables);

    backend->end();
}

void Renderer::resize(const win::Area<float> &area, const win::Dimensions<int> &res)
{
    backend->resize(area, res);
}
