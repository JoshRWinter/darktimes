#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLRendererBackend.hpp"
#include "../../TextureDefinitions.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: static_floor_renderer(roll)
	, static_atlas_renderer(roll)
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

	floor_texture = load_floor_textures(roll);
	atlases = load_atlases(roll);
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
	loaded_objects.clear();
	loaded_objects.reserve(statics.size());

	scratch.clear();

	for (const auto &r : statics)
		if (TextureDefinitions.textures[r.texture].atlas == -1)
			scratch.push_back(r);

	for (const auto &x : static_floor_renderer.load_static_floors(scratch, floor_layer_map))
	{
		auto &obj = loaded_objects.emplace_back();

		obj.type = GLLoadedObjectType::static_floor;
		obj.base_vertex = x;
	}

	scratch.clear();

	for (const auto &r : statics)
		if (TextureDefinitions.textures[r.texture].atlas != -1)
			scratch.push_back(r);

	for (const auto &x : static_atlas_renderer.load_static_atlas_items(scratch, atlases.at(0)))
	{
		auto &obj = loaded_objects.emplace_back();

		obj.type = GLLoadedObjectType::static_atlas;
		obj.base_vertex = x;
	}

	std::vector<const void*> v;
	v.reserve(loaded_objects.size());

	for (const auto &x : loaded_objects)
		v.push_back(&x);

	return v;
}

void GLRendererBackend::render(const std::vector<const void*> &objects)
{
	glClear(GL_COLOR_BUFFER_BIT);

	class DummyRenderer : public GLSubRenderer
	{
	public:
		void flush() override { }
	} dummy;

	GLSubRenderer *renderer = &dummy;
	for (const auto o : objects)
	{
		const auto &obj = *(GLLoadedObject*)o;

		switch (obj.type)
		{
			case GLLoadedObjectType::static_atlas:
				if (renderer != &static_atlas_renderer)
				{
					renderer->flush();
					renderer = &static_atlas_renderer;
				}

				static_atlas_renderer.add(obj.base_vertex);
				break;

			case GLLoadedObjectType::static_floor:
				if (renderer != &static_floor_renderer)
				{
					renderer->flush();
					renderer = &static_floor_renderer;
				}

				static_floor_renderer.add(obj.base_vertex);
				break;

			default: win::bug("wtf");
		}
	}

	renderer->flush();

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

std::vector<win::GLAtlas> GLRendererBackend::load_atlases(win::AssetRoll &roll)
{
	glActiveTexture(GL_TEXTURE0);
	std::vector<win::GLAtlas> result;
	for (const char *name : TextureDefinitions.atlases)
		result.emplace_back(roll[name], win::GLAtlas::Mode::linear);

	return result;
}

win::GLTexture GLRendererBackend::load_floor_textures(win::AssetRoll &roll)
{
}
