#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLRendererBackend.hpp"
#include "../../TextureDefinitions.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: floor_textures(roll)
	, atlases(roll)
	, static_floor_renderer(roll, floor_textures)
	, static_atlas_renderer(roll, atlases)
	, text_renderer(screen_dims, projection, GL_TEXTURE1, true)
{
	fprintf(stderr, "%s\n%s\n%s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	/*
	 * AYYYYYYYYYYYYY
	 * limit index depth
	 * limit atlas count
	 * fix 32bit bullshit
	 */

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
}

std::vector<const void*> GLRendererBackend::load_statics(const std::vector<Renderable> &statics)
{
	loaded_statics.clear();
	loaded_statics.reserve(statics.size());
	std::vector<const void*> result;

	for (const auto &r : statics)
		if (TextureDefinitions.textures[r.texture].atlas == -1)
			loaded_statics.emplace_back(GLLoadedObjectType::floor, static_floor_renderer.load(r));
		else
			loaded_statics.emplace_back(GLLoadedObjectType::atlas, static_atlas_renderer.load(r));

	static_floor_renderer.finalize();
	static_atlas_renderer.finalize();

	for (const auto &i : loaded_statics)
		result.push_back(&i);

	return result;
}

void GLRendererBackend::render_start()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLRendererBackend::render_end()
{
}

void GLRendererBackend::render_statics(const std::vector<const void*> &statics)
{
	for (const auto s : statics)
	{
		const auto object = *(const GLLoadedObject*)s;
		if (object.type == GLLoadedObjectType::floor)
			static_floor_renderer.add(object.base_vertex);
		else
			static_atlas_renderer.add(object.base_vertex);
	}

	static_floor_renderer.flush();
	static_atlas_renderer.flush();

#ifndef NDEBUG
	win::gl_check_error();
#endif
}
