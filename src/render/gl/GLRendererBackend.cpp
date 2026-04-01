#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLRendererBackend.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: projection(glm::ortho(projection.left, projection.right, projection.bottom, projection.top))
	, floor_renderer(roll, texture_map)
	, atlas_renderer(roll, texture_map)
	, text_renderer(screen_dims, projection, GL_TEXTURE1, true, 0, true)
{
	fprintf(stderr, "%s\n%s\n%s\n\n", (const char*)glGetString(GL_VENDOR), (const char*)glGetString(GL_RENDERER), (const char*)glGetString(GL_VERSION));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void GLRendererBackend::set_view(float x, float y, float zoom)
{
	const auto ident = glm::identity<glm::mat4>();
	const auto translate = glm::translate(ident, glm::vec3(-x, -y, 0.0f));
	const auto scale = glm::scale(ident, glm::vec3(zoom, zoom, 1.0f));
	const auto view = scale * translate;
	const auto vp = projection * view;

	floor_renderer.set_view_projection(vp);
	atlas_renderer.set_view_projection(vp);
}

void GLRendererBackend::load_statics(const std::vector<Renderable> &statics)
{
	loaded_statics.clear();

	std::vector<Renderable> floor_renderables, atlas_renderables;

	for (const auto &s : statics)
	{
		if (texture_map[s.texture].atlas_index == -1)
			floor_renderables.push_back(s);
		else
			atlas_renderables.push_back(s);
	}

	const auto floor_statics = floor_renderer.load(floor_renderables);
	const auto atlas_statics = atlas_renderer.load(atlas_renderables, texture_map);

	int floor_index = 0;
	int atlas_index = 0;
	for (const auto &s : statics)
	{
		if (texture_map[s.texture].atlas_index == -1)
			loaded_statics.emplace_back(StaticObject::Type::floor, floor_statics.at(floor_index++));
		else
			loaded_statics.emplace_back(StaticObject::Type::atlas, atlas_statics.at(atlas_index++));
	}
}

void GLRendererBackend::render_statics(const std::vector<int> &statics)
{
	if (statics.empty())
		return;

	renderable_ids.clear();
	renderable_ids.reserve(statics.size());

	auto type = loaded_statics[statics[0]].type;

	const auto flush = [this, &type]()
	{
		if (type == StaticObject::Type::floor)
			floor_renderer.render(renderable_ids);
		else
			atlas_renderer.render(renderable_ids);

		renderable_ids.clear();
	};

	for (const auto i : statics)
	{
		const auto &object = loaded_statics.at(i);

		if (type != object.type)
		{
			flush();
			type = object.type;
		}

		renderable_ids.push_back(object.base_vertex);
	}

	if (!renderable_ids.empty())
		flush();

	check_error();
}

void GLRendererBackend::render_dynamics(const std::vector<Renderable> &dynamics)
{

}

void GLRendererBackend::check_error()
{
#ifndef NDEBUG
	win::gl_check_error();
#endif
}
