#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll)
    : staging(100)
    , backend(new GLRendererBackend(area, res, roll))
{
}

void Renderer::set_leveldata(const LevelData &leveldata)
{
    static_renderables = leveldata.renderables;
    backend->load_statics(static_renderables, leveldata.occluders, leveldata.lights);
}

void Renderer::render(const Renderables &renderables)
{
    backend->set_view(renderables.centerx, renderables.centery, 1.0f); // 2.5f);
    backend->begin();

    staging.clear();
    for (const auto &r : static_renderables)
        staging.push_back(staging.size());

    backend->render_statics(staging);
    backend->render_dynamics(renderables.renderables);
    backend->render_lights(renderables.light_renderables);

    backend->end();
}

void Renderer::resize(const win::Area<float> &area, const win::Dimensions<int> &res)
{
    backend->resize(area, res);
}
