#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLRendererBackend.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: current_renderer(&dummy_renderer)
	, floor_textures(roll, texture_map)
	, atlases(roll, texture_map)
	, static_floor_renderer(roll)
	, static_atlas_renderer(roll)
	, dynamic_atlas_renderer(roll)
	, text_renderer(screen_dims, projection, GL_TEXTURE1, true)
{
	fprintf(stderr, "%s\n%s\n%s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	this->projection = glm::ortho(projection.left, projection.right, projection.bottom, projection.top);

	glClearColor(0.4f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

const win::Font &GLRendererBackend::create_font(win::Stream data, float size)
{
	glActiveTexture(GL_TEXTURE15);
	return fonts.add(std::move(text_renderer.create_font(size, std::move(data))));
}

void GLRendererBackend::draw_text(const win::Font &font, const char *text, float x, float y, bool centered)
{
	text_renderer.draw((win::GLFont&)font, text, x, y, centered);
	text_renderer.flush();
}

void GLRendererBackend::set_view(float x, float y, float zoom)
{
	const auto ident = glm::identity<glm::mat4>();
	const auto translate = glm::translate(ident, glm::vec3(-x, -y, 0.0f));
	const auto scale = glm::scale(ident, glm::vec3(zoom, zoom, 0.0f));
	const auto view = scale * translate;
	const auto vp = projection * view;

	static_floor_renderer.set_view_projection(vp);
	static_atlas_renderer.set_view_projection(vp);
	dynamic_atlas_renderer.set_view_projection(vp);
}

std::vector<const void*> GLRendererBackend::load_statics(const std::vector<Renderable> &statics)
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

	const auto floor_statics = static_floor_renderer.load(floor_renderables, floor_textures);
	const auto atlas_statics = static_atlas_renderer.load(atlas_renderables, atlases, texture_map);

	int floor_index = 0;
	int atlas_index = 0;
	for (const auto &s : statics)
	{
		if (texture_map[s.texture].atlas_index == -1)
			loaded_statics.emplace_back(GLLoadedObjectType::floor, floor_statics.at(floor_index++));
		else
			loaded_statics.emplace_back(GLLoadedObjectType::atlas, atlas_statics.at(atlas_index++));
	}

	std::vector<const void*> results;

	for (const auto &s : loaded_statics)
		results.push_back(&s);

	return results;
}

void GLRendererBackend::load_dynamics()
{
	dynamic_atlas_renderer.load_all(atlases, texture_map);
}

void GLRendererBackend::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLRendererBackend::render_end()
{
	current_renderer->flush();
}

void GLRendererBackend::render_statics(const std::vector<const void*> &statics)
{
	for (const auto s : statics)
	{
		const auto object = *(const GLLoadedObject*)s;
		if (object.type == GLLoadedObjectType::floor)
		{
			if (current_renderer != &static_floor_renderer)
			{
				current_renderer->flush();
				current_renderer = &static_floor_renderer;
			}

			static_floor_renderer.add(object.base_vertex);
		}
		else
		{
			if (current_renderer != &static_atlas_renderer)
			{
				current_renderer->flush();
				current_renderer = &static_atlas_renderer;
			}

			static_atlas_renderer.add(object.base_vertex);
		}
	}

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

void GLRendererBackend::render_dynamics(const std::vector<Renderable> &dynamics)
{
	for (const auto &d : dynamics)
		dynamic_atlas_renderer.add(d);

	current_renderer->flush();
	current_renderer = &dynamic_atlas_renderer;

#ifndef NDEBUG
	win::gl_check_error();
#endif
}
