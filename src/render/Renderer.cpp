#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: backend(new GLRendererBackend(screen_dims, projection, roll))
{
}

void Renderer::set_statics(const std::vector<Renderable> &statics)
{
	this->statics = statics;
	backend->load_statics(statics);
}

void Renderer::render(Renderables &renderables)
{
	if (renderables.centerx != 0.0f || renderables.centery != 0.0f)
		win::bug("bugly");
	backend->set_view(renderables.centerx, renderables.centery, 1.0);

	std::vector<int> ids;
	for (const auto &r : statics)
		ids.push_back(ids.size());

	backend->render_statics(ids);
}

