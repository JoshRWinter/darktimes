#include <chrono>

#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: backend(new GLRendererBackend(screen_dims, projection, roll))
	, font_title(backend->create_font(roll["font/CHE-THIS.TTF"], 0.8f))
	, font_debug(backend->create_font(roll["font/NotoSansMono-Regular.ttf"], 0.1f))
{
	backend->load_dynamics();
}

void Renderer::set_view(float x, float y, float zoom)
{
	backend->set_view(x, y, zoom);
}

void Renderer::set_statics(const std::vector<Renderable> &statics)
{
	static_objects = backend->load_statics(statics);
}

void Renderer::set_dynamics(const std::vector<Renderable> &dynamics)
{
	this->dynamics = dynamics;
}

void Renderer::render()
{
	backend->render_start();
	backend->render_statics(static_objects);
	backend->render_dynamics(dynamics);

	backend->draw_text(font_title, "DARK TIMES", 0.0f, 3.5f, true);

	static char fpsbuf[30] = "FPS: 69";
	static int total_fps = 0;
	static auto start = std::chrono::high_resolution_clock::now();
	const auto now = std::chrono::high_resolution_clock::now();
	++total_fps;
	if (std::chrono::duration<float, std::ratio<1, 1>>(now - start).count() > 1.0f)
	{
		start = now;
		snprintf(fpsbuf, sizeof(fpsbuf), "FPS: %d", total_fps);
		total_fps = 0;
	}

	backend->draw_text(font_debug, fpsbuf, -7.9f, 4.3f);
	backend->render_end();
}

