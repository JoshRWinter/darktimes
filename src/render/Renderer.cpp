#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
    : backend(new GLRendererBackend(screen_dims, projection, roll))
    , staging(100)
{
}

void Renderer::set_statics(const std::vector<Renderable> &statics)
{
    this->statics = statics;
    backend->load_statics(statics);
}

void Renderer::render(Renderables &renderables)
{
    backend->set_view(renderables.centerx, renderables.centery, 1.0);
    backend->begin();

    staging.clear();
    for (const auto &r : statics)
        staging.push_back(staging.size());

    backend->render_statics(staging);

    backend->end();
}

