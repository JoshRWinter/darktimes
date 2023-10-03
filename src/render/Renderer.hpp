#pragma once

#include <memory>
#include <vector>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "../Renderable.hpp"
#include "RendererBackend.hpp"

class Renderer
{
public:
	explicit Renderer(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll);

	void set_view(float x, float y, float zoom);
	void set_static_objects(const std::vector<Renderable> &objects);
	void set_dynamic_objects(const std::vector<Renderable> &objects);
	void render();

private:
	std::vector<const void*> static_objects;
	//std::vector<Renderable> dynamic_objects;
	std::unique_ptr<RendererBackend> backend;

	const win::Font &font_title;
	const win::Font &font_debug;
};
